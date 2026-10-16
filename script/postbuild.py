
import os
import shutil
import sys
import zipfile

def main():
	g_build_version: str = sys.argv[1]
	g_source_directory: str = sys.argv[2]
	g_runtime_directory: str = sys.argv[3]
	g_is_build_debug: str = sys.argv[4]

	__debug: bool = (g_is_build_debug == "1")

	__shipping_folder_name: str = str.format("{}/loader-v{}-{}",
		g_runtime_directory,
		g_build_version,
		"debug" if __debug else "release"
	)

	if not os.path.exists(__shipping_folder_name):
		os.makedirs(__shipping_folder_name)

	if __debug:
			g_runtime_directory += "/Debug"

	__catch_error_dll_name = "catch_error_mini_d.dll" if __debug else "catch_error_mini_d.dll"

	shutil.copyfile(
		f"{g_runtime_directory}/loader.dll", 
		f"{__shipping_folder_name}/loader.dll"
	)

	shutil.copyfile(
		f"{g_source_directory}/ext/{__catch_error_dll_name}", 
		f"{__shipping_folder_name}/catch_error_mini.dll"
	)

	if __debug:
		shutil.copyfile(
				f"{g_runtime_directory}/loader.pdb", 
				f"{__shipping_folder_name}/loader.pdb"
			)

	shutil.make_archive(__shipping_folder_name, 'zip', __shipping_folder_name)

if (__name__ == "__main__"):
	main()