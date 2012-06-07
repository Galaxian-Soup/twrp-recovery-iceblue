#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
//#include <reboot/reboot.h>
#include <unistd.h>

#include "tw_reboot.h"
#include "recovery_ui.h"
#include "roots.h"
#include "extra-functions.h"

// isRebootCommandSupported: Return 1 if command is supported, 0 if the command is not supported, -1 on error
int tw_isRebootCommandSupported(RebootCommand command)
{
    switch (command)
    {
    case rb_system:
    case rb_recovery:
    case rb_poweroff:
    case rb_bootloader:
        return 1;

    default:
        return 0;
    }
    return -1;
}

// setRebootMode: Set the reboot state (without rebooting). Return 0 on success, -1 on error or unsupported
int tw_setRebootMode(RebootCommand command)
{
    return -1;
}

// reboot: Reboot the system. Return -1 on error, no return on success
int tw_reboot(RebootCommand command)
{
    // Always force a sync before we reboot
    sync();

    ensure_path_unmounted("/sdcard");

    switch (command)
    {
    case rb_current:
    case rb_system:
        finish_recovery("s");
		sync();
		check_and_run_script("/sbin/rebootsystem.sh", "reboot system");
        return reboot(RB_AUTOBOOT);
    case rb_recovery:
		check_and_run_script("/sbin/rebootrecovery.sh", "reboot recovery");
        return __reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART2, (void*) "recovery");
    case rb_bootloader:
		check_and_run_script("/sbin/rebootbootloader.sh", "reboot bootloader");
        return __reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART2, (void*) "bootloader");
    case rb_poweroff:
		check_and_run_script("/sbin/poweroff.sh", "power off");
        return reboot(RB_POWER_OFF);

    default:
        return -1;
    }
    return -1;
}

