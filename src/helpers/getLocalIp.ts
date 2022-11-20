import os from "os";

export function getLocalIp() {
    let localIp = "127.0.0.1";
    const interfaces = os.networkInterfaces();
    Object.keys(interfaces).forEach((interfaceName) => {
        for (const iface of interfaces[interfaceName] ?? []) {
            // Ignore IPv6 and 127.0.0.1
            if (iface.family !== "IPv4" || iface.internal !== false) {
                continue;
            }
            // Set the local ip to the first IPv4 address found and exit the loop
            localIp = iface.address;
            return;
        }
    });
    return localIp;
}
