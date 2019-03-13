import { run } from 'uebersicht';

const COMMAND = 'osascript ./lib/spotify.osa';

function spotify() {
    return run(COMMAND).then(JSON.parse).then(data => {
        data.duration /= 1000;
        return data;
    });
}

spotify.command = COMMAND;

export default spotify;
