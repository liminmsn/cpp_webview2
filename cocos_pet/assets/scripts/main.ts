import { _decorator, Component, Node, Quat, UITransform, Vec3 } from 'cc';
const { ccclass, property, type } = _decorator;

@ccclass('main')
export class main extends Component {
    start() {
        if (document.body) {
            setTimeout(() => {
                document.body.style.backgroundColor = '#ffffff00';
                document.body.style['app-region'] = 'drag';
            }, 0);
        }
    }
}


