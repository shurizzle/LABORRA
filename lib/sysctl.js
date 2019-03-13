import { run } from 'uebersicht';

const sysctl = {};

function populate(res) {
    res.trim().split(/(?:\r?\n)+/).forEach(line => {
        const index = line.indexOf(':');
        if (index !== -1) {
            const key = line.slice(0, index).trim();
            const value = line.slice(index + 1).trim();
            const path = key.split('.');
            let pos = sysctl;

            let i;
            for (i = 0; i < path.length - 1; ++i) {
                pos = pos[path[i]] = pos[path[i]]||{};
            }
            pos[path[i]] = value;
        }
    });
}

sysctl.init = function init() {
    return run('sysctl -a').then(populate);
}

export default sysctl;
