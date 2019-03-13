import { run } from 'uebersicht';
import shellEscape from './shell-escape';

function extractUsed(props) {
    if (props['Volume Used Space']) {
        const matches = props['Volume Used Space'].match(/\(([0-9]+(?:\.[0-9]+)?)%\)$/);
        if (matches) {
            return parseFloat(matches[1]);
        }
    }
}

function split(output) {
    const lines = output.split(/\n+/),
        props = {};

    for (var i = 0; i < lines.length; ++i) {
        const line = lines[i].trim();
        if (line.length > 0) {
            const index = line.indexOf(':');
            if (index >= 0) {
                const key = line.substring(0, index).trim();
                const value = line.substring(index + 1).trim();
                props[key] = value;
            }
        }
    }

    return props;
}

function getProps(id) {
    return run(shellEscape(['diskutil', 'info', id])).then(split);
}

export function used(id) {
    return getProps(id).then(extractUsed);
}
