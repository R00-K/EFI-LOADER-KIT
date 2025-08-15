#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h> // Required for EFI_FILE_PROTOCOL

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    EFI_HANDLE NtfsImageHandle;
    CHAR16 *NtfsDriverPath = L"\\EFI\\Boot\\Ntfs.efi";

    // Get the Loaded Image Protocol for our own image
    Status = gBS->OpenProtocol(
        ImageHandle,
        &gEfiLoadedImageProtocolGuid,
        (VOID **)&LoadedImage,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
        Print(L"[-] Failed to get LoadedImage protocol: %r\n", Status);
        return Status;
    }

    // Create a device path to Ntfs.efi
    DevicePath = FileDevicePath(LoadedImage->DeviceHandle, NtfsDriverPath);
    if (DevicePath == NULL) {
        Print(L"[-] Failed to create device path for Ntfs.efi\n");
        return EFI_NOT_FOUND;
    }

    // Load Ntfs.efi
    Status = gBS->LoadImage(FALSE, ImageHandle, DevicePath, NULL, 0, &NtfsImageHandle);
    if (EFI_ERROR(Status)) {
        Print(L"[-] Failed to load Ntfs.efi: %r\n", Status);
        return Status;
    }

    // Start Ntfs.efi
    Status = gBS->StartImage(NtfsImageHandle, NULL, NULL);
    if (EFI_ERROR(Status)) {
        Print(L"[-] Failed to start Ntfs.efi: %r\n", Status);
        return Status;
    }

    Print(L"[+] Ntfs.efi loaded and started successfully.\n");

    // Connect all controllers to make new filesystems visible
    UINTN ControllerHandleCount = 0;
    EFI_HANDLE *ControllerHandleBuffer = NULL;

    Status = gBS->LocateHandleBuffer(
        AllHandles,
        NULL,
        NULL,
        &ControllerHandleCount,
        &ControllerHandleBuffer
    );

    if (!EFI_ERROR(Status)) {
        for (UINTN i = 0; i < ControllerHandleCount; i++) {
            gBS->ConnectController(ControllerHandleBuffer[i], NULL, NULL, TRUE);
        }
        Print(L"[+] Connected all controllers.\n");
    } else {
        Print(L"[-] Failed to locate all handles: %r\n", Status);
    }

    // Locate file systems (after connecting)
    UINTN FsHandleCount = 0;
    EFI_HANDLE *FsHandleBuffer = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
    EFI_FILE_PROTOCOL *Root;
    EFI_FILE_PROTOCOL *File;

    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &FsHandleCount,
        &FsHandleBuffer
    );
    if (EFI_ERROR(Status)) {
        Print(L"[-] Failed to locate file system handles: %r\n", Status);
        return Status;
    }

    for (UINTN i = 0; i < FsHandleCount; ++i) {
        Status = gBS->HandleProtocol(
            FsHandleBuffer[i],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID **)&Volume
        );
        if (EFI_ERROR(Status)) continue;

        Status = Volume->OpenVolume(Volume, &Root);
        if (EFI_ERROR(Status)) continue;

        Status = Root->Open(
            Root,
            &File,
            L"Windows\\System32\\config\\SYSTEM",
            EFI_FILE_MODE_READ,
            0
        );

        if (!EFI_ERROR(Status)) {
            Print(L"[✔] SYSTEM hive found on volume %u!\n", i);
            File->Close(File);
        } else {
            Print(L"[✘] SYSTEM hive not found on volume %u\n", i);
        }
        
        
        
    
    
    
    
    
    
    
        Root->Close(Root);
    }

    // Cleanup
    if (ControllerHandleBuffer) {
        FreePool(ControllerHandleBuffer);
    }
    if (FsHandleBuffer) {
        FreePool(FsHandleBuffer);
    }

    return EFI_SUCCESS;
}

