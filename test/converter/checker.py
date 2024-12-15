import asyncio
from enum import Enum
from pathlib import Path
import sys
from typing import Final, Optional

class TermColor(Enum):
    RED = "\033[91m"
    GREEN = "\033[92m"
    RESET = "\033[0m"


class TestsDir(Enum):
    OK = "ok"
    NOT_OK = "not_ok"
    TWICE = "twice"


def add_color(message: str, color: TermColor) -> str:
    return f"{color.value}{message}{TermColor.RESET.value}"


MSG_OK: Final[str] = add_color("OK", TermColor.GREEN)
MSG_FAILED: Final[str] = add_color("FAILED", TermColor.RED)
MSG_PASSED: Final[str] = add_color("PASSED", TermColor.GREEN)

INPUT_IMAGE: Final[str] = "image.bmp"
OUTPUT_IMAGE: Final[str] = "image_neg.bmp"
REFERENCE_IMAGE: Final[str] = "image_neg_reference.bmp"
OUTPUT_TWICE_IMAGE: Final[str] = "image_neg_twice.bmp"

IMAGES_SAME_MESSAGE: Final[str] = "Images are same"

# Путь для сохранения некорректных изображений
FAILED_OUTPUT_DIR: Final[Path] = Path("failed_outputs")
FAILED_OUTPUT_DIR.mkdir(exist_ok=True)


class BMPFileHeader:
   def __init__(self):
     self.bfType = 0
     self.bfSize = 0
     self.bfReserved1 = 0
     self.bfReserved2 = 0
     self.bfOffBits = 0

class BMPInfoHeader:
    def __init__(self):
      self.biSize = 0
      self.biWidth = 0
      self.biHeight = 0
      self.biPlanes = 0
      self.biBitCount = 0
      self.biCompression = 0
      self.biSizeImage = 0
      self.biXPelsPerMeter = 0
      self.biYPelsPerMeter = 0
      self.biClrUsed = 0
      self.biClrImportant = 0
   
class BMPImage:
    def __init__(self):
        self.header = BMPFileHeader()
        self.info = BMPInfoHeader()
        self.palette = None
        self.data = None
def bmp_read(filename, image:BMPImage):
      
      with open(filename,"rb") as file:
         image.header.bfType = int.from_bytes(file.read(2), byteorder='little')
         image.header.bfSize = int.from_bytes(file.read(4), byteorder='little')
         image.header.bfReserved1 = int.from_bytes(file.read(2), byteorder='little')
         image.header.bfReserved2 = int.from_bytes(file.read(2), byteorder='little')
         image.header.bfOffBits = int.from_bytes(file.read(4), byteorder='little')
         
         image.info.biSize = int.from_bytes(file.read(4), byteorder='little')
         image.info.biWidth = int.from_bytes(file.read(4), byteorder='little', signed = True)
         image.info.biHeight = int.from_bytes(file.read(4), byteorder='little', signed = True)
         image.info.biPlanes = int.from_bytes(file.read(2), byteorder='little')
         image.info.biBitCount = int.from_bytes(file.read(2), byteorder='little')
         image.info.biCompression = int.from_bytes(file.read(4), byteorder='little')
         image.info.biSizeImage = int.from_bytes(file.read(4), byteorder='little')
         image.info.biXPelsPerMeter = int.from_bytes(file.read(4), byteorder='little', signed = True)
         image.info.biYPelsPerMeter = int.from_bytes(file.read(4), byteorder='little', signed = True)
         image.info.biClrUsed = int.from_bytes(file.read(4), byteorder='little')
         image.info.biClrImportant = int.from_bytes(file.read(4), byteorder='little')

         if image.info.biBitCount == 8:
            palette_size =  image.info.biClrUsed if  image.info.biClrUsed != 0 else 256
            image.palette = file.read(palette_size * 4)

         data_offset = image.header.bfOffBits
         file.seek(data_offset)
         image.data = file.read()
         

def bmp_free(image:BMPImage):
      if image.palette:
         image.palette = None
      if image.data:
         image.data = None

async def run_all_tests(test_data_dir: Path, converter: Path, comparer: Path) -> int:
    ok_tests_dir = test_data_dir / TestsDir.OK.value
    not_ok_tests_dir = test_data_dir / TestsDir.NOT_OK.value
    twice_tests_dir = test_data_dir / TestsDir.TWICE.value
    failed_tests = await asyncio.gather(
        run_ok_tests(ok_tests_dir, converter, comparer),
        run_not_ok_tests(not_ok_tests_dir, converter),
        run_twice_tests(twice_tests_dir, converter, comparer),
    )
    return sum(failed_tests)


async def run_ok_tests(tests_dir: Path, converter: Path, comparer: Path) -> int:
    failed_tests = 0
    for test in sorted(tests_dir.iterdir(), key=lambda p: p.stem):
        test_name = test.stem
        image_in = test / INPUT_IMAGE
        image_out = test / OUTPUT_IMAGE
        image_out_ref = test / REFERENCE_IMAGE

        rc_actual, out_actual, err_actual = await run_test(
            converter,
            (image_in, image_out)
        )

        test_failed = False
        if not await validate_ok_test(
            test_name,
            comparer,
            (image_out, image_out_ref),
            (0, rc_actual),
            ("", out_actual),
            ("", err_actual),
            image_in,  # Pass also input for additional checks.
            image_out,  # Pass output image too.
            False # indicate that we are in initial call
        ):
            test_failed = True

        if test_failed:
            failed_tests += 1
            # Сохранение некорректного изображения
            if image_out.exists():
                dest = FAILED_OUTPUT_DIR / f"{test_name}_output.bmp"
                image_out.rename(dest)
                print(f"Saved failed output to {dest}", file=sys.stderr)

        # Удаление выходного файла
        if image_out.exists():
            image_out.unlink()
    return failed_tests


async def run_not_ok_tests(tests_dir: Path, converter: Path) -> int:
    failed_tests = 0
    for test in sorted(tests_dir.iterdir(), key=lambda p: p.stem):
        test_name = test.stem
        image_in = test / INPUT_IMAGE
        image_out = test / OUTPUT_IMAGE

        rc_actual, out_actual, err_actual = await run_test(
            converter,
            (image_in, image_out)
        )

        if not validate_not_ok_test(
            test_name,
            image_out,
            (1, rc_actual),
            ("", out_actual),
            ("", err_actual),
        ):
            failed_tests += 1
            # Сохранение ошибочного изображения, если оно было создано
            if image_out.exists():
                dest = FAILED_OUTPUT_DIR / f"{test_name}_unexpected_output.bmp"
                image_out.rename(dest)
                print(f"Saved unexpected output to {dest}", file=sys.stderr)

        # Удаление выходного файла
        if image_out.exists():
            image_out.unlink()
    return failed_tests


async def run_twice_tests(tests_dir: Path, converter: Path, comparer: Path) -> int:
    failed_tests = 0
    for test in sorted(tests_dir.iterdir(), key=lambda p: p.stem):
        test_name = test.stem
        image_in = test / INPUT_IMAGE
        image_out = test / OUTPUT_IMAGE
        image_out_twice = test / OUTPUT_TWICE_IMAGE

        rc_actual, out_actual, err_actual = await run_test(
            converter,
            (image_in, image_out)
        )
        rc_twice, out_twice, err_twice = await run_test(
            converter,
            (image_out, image_out_twice)
        )

        # Проверка после двух инвертаций (должно вернуться к исходному изображению)
        if not await validate_ok_test(
            test_name,
            comparer,
            (image_in, image_out_twice),
            (0, rc_actual),
            ("", out_actual),
            ("", err_actual),
           image_in,  # Pass also input for additional checks.
           image_out_twice, # Pass the output of twice execution of the converter.
           True
        ):
            failed_tests += 1
            # Сохранение некорректного изображения после двух инвертаций
            if image_out_twice.exists():
                dest = FAILED_OUTPUT_DIR / f"{test_name}_twice_output.bmp"
                image_out_twice.rename(dest)
                print(f"Saved twice failed output to {dest}", file=sys.stderr)

        # Удаление выходных файлов
        if image_out.exists():
            image_out.unlink()
        if image_out_twice.exists():
            image_out_twice.unlink()
    return failed_tests


async def run_test(
    converter: Path,
    images: tuple[Path, Path],
) -> tuple[int, str, str]:
    image_in, image_out = images
    if image_out.exists():
        image_out.unlink()
    proc = await asyncio.create_subprocess_exec(
        converter,"--silent", str(image_in), str(image_out),
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )

    stdout, stderr = await proc.communicate()
    rc = proc.returncode
    out_text = stdout.decode("ascii", "replace").strip()
    err_text = stderr.decode("ascii", "replace").strip()
    return rc, out_text, err_text


async def validate_ok_test(
    test_name: str,
    comparer: Path,
    imgs: tuple[Path, Path],
    rcs: tuple[int, int],
    outs: tuple[str, str],
    errs: tuple[str, str],
    image1_path : Path,
    image2_path : Path,
    twice_call:bool
) -> bool:
    rc_expected, rc_actual = rcs
    if rc_expected != rc_actual:
        print(f"{MSG_FAILED} {test_name}: Return code {rc_expected} != {rc_actual}", file=sys.stderr)
        print(f"  stdout: '{outs[1]}'", file=sys.stderr)
        print(f"  stderr: '{errs[1]}'", file=sys.stderr)
        return False

    out_expected, out_actual = outs
    if out_expected != out_actual:
        print(f"{MSG_FAILED} {test_name}: incorrect stdout", file=sys.stderr)
        print(f"  Expected stdout: '{out_expected}'", file=sys.stderr)
        print(f"  Actual stdout: '{out_actual}'", file=sys.stderr)
        return False

    err_expected, err_actual = errs
    if err_expected != err_actual:
        print(f"{MSG_FAILED} {test_name}: incorrect stderr", file=sys.stderr)
        print(f"  Expected stderr: '{err_expected}'", file=sys.stderr)
        print(f"  Actual stderr: '{err_actual}'", file=sys.stderr)
        return False

    # Запуск comparer для проверки изображений
    proc = await asyncio.create_subprocess_exec(
        comparer, str(imgs[0]), str(imgs[1]),
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )

    stdout, stderr = await proc.communicate()
    rc = proc.returncode
    out_text = stdout.decode("ascii", "replace").strip()
    err_text = stderr.decode("ascii", "replace").strip()

    if rc != 0 or out_text != IMAGES_SAME_MESSAGE:
        print(f"{MSG_FAILED} {test_name}: result image is incorrect", file=sys.stderr)
        print(f"  Comparer return code: {rc}", file=sys.stderr)
        print(f"  Comparer stdout: '{out_text}'", file=sys.stderr)
        print(f"  Comparer stderr: '{err_text}'", file=sys.stderr)

        # Дополнительная информация о изображениях
        image1 = BMPImage()
        image2 = BMPImage()
        try:
             bmp_read(str(image1_path), image1)
             bmp_read(str(image2_path), image2)
            
             print(f"  Image 1: Width={image1.info.biWidth}, Height={image1.info.biHeight}, BitCount={image1.info.biBitCount}, biSizeImage={image1.info.biSizeImage}", file=sys.stderr)
             print(f"  Image 2: Width={image2.info.biWidth}, Height={image2.info.biHeight}, BitCount={image2.info.biBitCount}, biSizeImage={image2.info.biSizeImage}", file=sys.stderr)

             if image1.info.biBitCount == 8 and image1.palette and image2.info.biBitCount == 8 and image2.palette:
                if twice_call is False: # if not twice test
                    
                   print(f"  Image 1 Palette: {image1.palette}", file=sys.stderr)

                   
                else:
                  
                    # if twice test, check for original and modified palettes after double swaps
                   temp_image1 = BMPImage()
                   bmp_read(str(image1_path), temp_image1)
                  
                   
                   print(f" Image 1 original Palette : {temp_image1.palette}", file = sys.stderr)
                   print(f"  Image 2 Palette: {image2.palette}", file=sys.stderr)

        except Exception as e:
            print(f"  Error reading images: {e}", file=sys.stderr)
            return False
        finally:
           bmp_free(image1)
           bmp_free(image2)
           if 'temp_image1' in locals():
               bmp_free(temp_image1)


        return False

    print(f"{MSG_OK} {test_name}")
    return True


def validate_not_ok_test(
    test_name: str,
    image_out: Path,
    rcs: tuple[int, int],
    outs: tuple[Optional[str], str],
    errs: tuple[str, str],
) -> bool:
    rc_expected, rc_actual = rcs
    if rc_expected != rc_actual:
        print(f"{MSG_FAILED} {test_name}: Return code {rc_expected} != {rc_actual}, expected {rc_expected}", file=sys.stderr)
        print(f"  stdout: '{outs[1]}'", file=sys.stderr)
        print(f"  stderr: '{errs[1]}'", file=sys.stderr)
        return False

    out_expected, out_actual = outs
    if out_expected != out_actual:
        print(f"{MSG_FAILED} {test_name}: incorrect stdout, expected '{out_expected}'", file=sys.stderr)
        print(f"  Actual stdout: '{out_actual}'", file=sys.stderr)
        return False

    _, err_actual = errs
    if not err_actual:
        print(f"{MSG_FAILED} {test_name}: incorrect stderr (expected error message), got empty error", file=sys.stderr)
        return False

    if image_out.exists():
        print(f"{MSG_FAILED} {test_name}: output file created in incorrect scenario, output file was not expected", file=sys.stderr)
        return False

    print(f"{MSG_OK} {test_name}")
    return True


def main() -> int:
    if len(sys.argv) != 4:
        print("Usage: test.py <converter> <test_data_dir> <comparer>", file=sys.stderr)
        return 1

    converter = Path(sys.argv[1]).absolute()
    tests_dir = Path(sys.argv[2]).absolute()
    comparer = Path(sys.argv[3]).absolute()

    if not converter.exists() or not converter.is_file():
        print(f"Converter executable '{converter}' does not exist or is not a file.", file=sys.stderr)
        return 1

    if not comparer.exists() or not comparer.is_file():
        print(f"Comparer executable '{comparer}' does not exist or is not a file.", file=sys.stderr)
        return 1

    if not tests_dir.exists() or not tests_dir.is_dir():
        print(f"Test data directory '{tests_dir}' does not exist or is not a directory.", file=sys.stderr)
        return 1

    failed_tests: int = asyncio.run(run_all_tests(tests_dir, converter, comparer))
    if failed_tests > 0:
        print(f"{failed_tests} tests {MSG_FAILED}\n")
        return 1
    print(f"All tests {MSG_PASSED}\n")
    return 0


if __name__ == "__main__":
    rc = main()
    sys.exit(rc)