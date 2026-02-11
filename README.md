# trackpad-is-too-damn-big

Trackpad Is Too Damn Big (TITDB) is a utility designed to customize trackpad behavior on Linux. TITDB creates a virtual trackpad device and forwards input events from the selected trackpad device to it while preventing other applications from receiving events from the original trackpad device and modifying the events to achieve the desired functionality. TITDB currently supports virtually reducing trackpad size in two different modes and one additional mode for printing input events without modifying them. TITDB has a very small memory and CPU footprint and is designed to be easily expandable for integrating new functionality in the future.

## Supported modes 

- **Print**: Print selected device properties and incoming events from the original device without modifying them.
- **Strict**: Completely disable designated areas of the trackpad based on specified percentages from the edges.
- **Flex**: Disable initial trackpad input from designated areas while allowing re-entry from valid areas and recognition of multitouch gestures. This ensures that the entire trackpad area remains usable and gestures are registered correctly without any restrictions.

## Troubleshooting
Because TITDB prevents other applications from receiving events from the original device, applications that specifically monitor the original device will not work as intended while TITDB is running. This can be easily resolved by configuring the applications to monitor the virtual device that TITDB creates instead of the original device.

## Building
You can build TITDB with the following commands 
```bash
git clone https://github.com/tascvh/trackpad-is-too-damn-big.git
cd trackpad-is-too-damn-big
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make
```

## Running 

```bash
sudo  titdb -d /dev/input/event0
```
Replace /dev/input/event0 with your trackpad device filename if necessary

### Running without sudo

TITDB needs to be able to access the `input` and `uinput` subsystem to work. To run TITDB without sudo, your user needs to have permissions. To achieve this, take the following steps:

1. Make sure the `uinput` group exists
```bash
sudo groupadd --system uinput
```

The `--system` flag is needed for systemd.
For other init systems, you may try to create the `uinput` group without `--system`. If this does not work, you can delete `uinput` group with `sudo groupdel uinput` and recreate the group with the `--system` flag.

2. Add your user to the `input` and the `uinput` group:
```bash
sudo usermod -aG input,uinput username
```

Make sure that it's effective by running `groups`. You might have to logout and login.
In some cases a reboot may also be required.

3. Make sure the uinput device file has the right permissions:
Add a udev rule (in either `/etc/udev/rules.d` or `/lib/udev/rules.d`) with the following content:
```bash
KERNEL=="uinput", MODE="0660", GROUP="uinput", OPTIONS+="static_node=uinput"
```

You will need to restart `udevd` via
```bash
sudo systemctl restart systemd-udevd.service
```
to get the new permissions.

4. Make sure the `uinput` drivers are loaded.
You will probably have to run this command whenever you start TITDB for the first time.
```bash
sudo modprobe uinput
```

Credit: https://github.com/kmonad/kmonad/blob/master/doc/faq.md#linux
