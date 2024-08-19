import ctypes


so_file = "C:/Users/Admin/STM32CubeIDE/workspace_1.16.0/ScanADCMulti/Debug/libScanADCMulti.dll"
# so_file = "Debug/libScanADCMulti.so"
my_function = ctypes.CDLL(so_file,winmode=0)

my_function.ADS125x_Start_thread()