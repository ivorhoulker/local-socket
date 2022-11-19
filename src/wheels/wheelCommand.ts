import { Vector4 } from "./type/Vector4";

export const getWheelCommand = (vector: Vector4 = [0, 0, 0, 0]) => {
    const [wheelSpeedA, wheelSpeedB, wheelSpeedC, wheelSpeedD] = vector;

    const f = 3;
    const length = 8;
    const speedWheelMode = 0x02;
    const absWheelSpeedA = Math.abs(wheelSpeedA) & 0xff;
    const absWheelSpeedB = Math.abs(wheelSpeedB) & 0xff;
    const absWheelSpeedC = Math.abs(wheelSpeedC) & 0xff;
    const absWheelSpeedD = Math.abs(wheelSpeedD) & 0xff;
    const wheelSpeedReserved1 = 0x00;
    const wheelSpeedReserved2 = 0x00;
    const wheelADirection = wheelSpeedA < 0 ? 1 : 0;
    const wheelBDirection = wheelSpeedB < 0 ? 1 : 0;
    const wheelCDirection = wheelSpeedC < 0 ? 1 : 0;
    const wheelDDirection = wheelSpeedD < 0 ? 1 : 0;
    const wheelSpeedADirection = wheelADirection << 0; // 0为正转,0x01为反转
    const wheelSpeedBDirection = wheelBDirection << 1; // 0为正转,0x02为反转
    const wheelSpeedCDirection = wheelCDirection << 2; // 0为正转,0x04为反转 //this is why comments like this suck
    const wheelSpeedDDirection = wheelDDirection << 3; // 0为正转,0x04为反转

    const wheelSpeedDirection =
        wheelSpeedADirection | wheelSpeedBDirection | wheelSpeedCDirection | wheelSpeedDDirection;
    const checksum =
        (f +
            length +
            speedWheelMode +
            absWheelSpeedA +
            absWheelSpeedB +
            absWheelSpeedC +
            absWheelSpeedD +
            wheelSpeedDirection) &
        0xff;
    const cmd = [
        0xff,
        0xfe,
        f,
        length,
        speedWheelMode,
        absWheelSpeedA,
        absWheelSpeedB,
        absWheelSpeedC,
        absWheelSpeedD,
        wheelSpeedReserved1,
        wheelSpeedReserved2,
        wheelSpeedDirection,
        checksum,
    ];
    console.log('wheels::', cmd.join());
    return Buffer.from(cmd);
};
