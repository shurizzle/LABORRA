import { css } from 'uebersicht';

const meterDefaultStyle = css({
    display: 'inline-block',
    width: '5em',
    height: '1em',

    '& > div': {
        backgroundColor: 'lime',
    },
});

function Meter ({ min, max, value, className, ...props }) {
    className = className ? `${meterDefaultStyle} ${className}` : meterDefaultStyle;

    return (
        <div className={className} { ...props }>
            <div style={{ width: `${100 / (max - min) * (value - min)}%`, height: '100%' }} />
        </div>
    );
}

export default Meter;
