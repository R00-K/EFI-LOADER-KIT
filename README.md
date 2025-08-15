# EFI Loader Kit

## Overview

EFI Loader Kit is a C++ EFI application that hooks into the UEFI boot process by replacing the default `bootmgfw.efi` with a custom loader. It provides low-level system access, direct NTFS parsing, and registry manipulation during early boot stages, allowing for advanced payload deployment and experimentation within the EFI environment.

**Technical Features:**

* Rename the original `bootmgfw.efi` and deploy a custom EFI binary in its place.
* EFI natively gates only FAT32 partitions; to access NTFS partitions, the NTFS driver must be manually initialized and loaded.
* Parse and access the `SYSTEM` registry hive (`C:\Windows\System32\Config`) directly from EFI memory space.
* Modify targeted DWORD values for configuration changes.
* First-stage payload: allocate physical memory pages, set up function pointers, and launch a second-stage payload optimized to operate when Windows Defender is inactive.
* Chainload the original EFI to maintain normal boot sequence.

> Note: Parsing and registry operations can be implemented by converting CHINTPW logic to EFI-compatible routines.

## Workflow

1. Copy the custom EFI and rename the original `bootmgfw.efi`.
2. Initialize NTFS driver structures to access the NTFS system partition (EFI only provides native FAT32 access).
3. Load and parse the `SYSTEM` registry hive to modify DWORD values.
4. Allocate memory regions and prepare execution context for the second-stage payload.
5. Launch second-stage payload and chainload original EFI to continue normal boot.

## Disclaimer

This project is intended for educational and research purposes only. Direct manipulation of EFI files and registry hives can render a system unbootable. Use at your own risk.

## Building

* Requires a UEFI development environment (e.g., EDK2) and a C++ compiler capable of producing EFI binaries.
* Follow standard EFI application compilation procedures for your environment.

## License

[MIT License](LICENSE)
