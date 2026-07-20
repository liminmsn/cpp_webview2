import { _decorator, Component, Node, Quat, Vec3 } from 'cc';
const { ccclass, property } = _decorator;

@ccclass('fromAxisAngle')
export class fromAxisAngle extends Component {
    start() {

    }

    private rotationSpeed: number = 90;
    update(deltaTime: number) {
        // 每帧计算旋转角度
        const angle = this.rotationSpeed * deltaTime;
        // 绕 Y 轴旋转
        const q = new Quat();
        Quat.fromAxisAngle(q, Vec3.UP, angle * Math.PI / 180);
        this.node.rotate(q);
    }
}


