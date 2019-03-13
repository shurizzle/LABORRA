import { css, styled } from 'uebersicht';
import spotify from './lib/spotify';
import { used } from './lib/storage';

import Meter from './lib/meter';

import SpotifyIcon from './lib/icons/spotify';

const SIZE = 11;
const PADDING = 5;

const spotifyIconClass = css({
    marginRight: PADDING,
});

const meterClass = css({
    height: SIZE,
    width: '10%',
    boxSizing: 'border-box',
    border: '1px solid #abb2bf',
    marginLeft: PADDING,

    '& > div': {
        backgroundColor: '#abb2bf',
    },
});

const Root = styled('div')({
    position: 'relative',
    height: SIZE,
});

const Track = styled('marquee')({
    display: 'inline-block',
    maxWidth: '20%',
    whiteSpace: 'nowrap',
    overflowX: '-webkit-marquee',
    WebkitMarqueeDirection: 'backwards',
    WebkitMarqueeIncrement: `6px`,
    WebkitMarqueeRepetition: 'infinite',
    WebkitMarqueeSpeed: 'slow',
    WebkitMarqueeStyle: 'scroll',
});

export const refreshFrequency = false;

export const initialState = {
    spotify: null,
    storage: {},
};

function updateDiskAction(name, usage) {
    return {
        type: 'DISK_UPDATE',
        name,
        usage,
    };
}

function updateStorage(dispatch) {
    const fn = () => {
        used('/').then(usage => dispatch(updateDiskAction('root', usage)))
            .catch(() => dispatch(updateDiskAction('root', null)))
            .then(() => {
                used('72920524-97C6-3751-BB67-B400F17DF049')
                    .then(usage => dispatch(updateDiskAction('ext', usage)))
                    .catch(() => dispatch(updateDiskAction('ext', null)))
                    .then(() => setTimeout(fn, 10000));
            });
    };

    return fn;
}

export const init = (dispatch) => {
    setInterval(() => {
        spotify().then(data => dispatch({type: 'SPOTIFY_UPDATE', data})).
            catch(() => dispatch({type: 'SPOTIFY_UPDATE', data: null}));
    }, 1000);

    setTimeout(updateStorage(dispatch), 10000);
};

export const className = {
    fontFamily: 'Menlo, Consolas, DejaVu Sans Mono, monospace',
    lineHeight: `${SIZE}px`,
    fontSize: SIZE,
    color: '#abb2bf',
    boxSizing: 'border-box',
    bottom: 0,
    left: 0,
    width: '100%',
    padding: PADDING,
    height: PADDING * 2 + SIZE,
    backgroundColor: '#333',
};

const Spotify = props => {
    if (props && (props.state === 'playing' || props.state === 'paused')) {
        const track = `${props.artist} - ${props.name}`;
        return (
            <div>
                <SpotifyIcon className={spotifyIconClass} />
                <Track>{track}</Track>
                <Meter className={meterClass} min={0} max={props.duration} value={props.position} />
            </div>
        );
    }

    return null;
};

export const render = (state) => {
    return (
        <Root>
            <Spotify { ...state.spotify } />
        </Root>
    );
}

export const updateState = (event, previousState) => {
    if (event.type === 'SPOTIFY_UPDATE') {
        return {
            ...previousState,
            spotify: event.data,
        };
    } else if (event.type === 'DISK_UPDATE') {
        return {
            ...previousState,
            storage: {
                ...previousState.storage,
                [event.name]: event.usage,
            },
        };
    }

    return previousState;
}
