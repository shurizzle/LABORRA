import { css } from 'uebersicht';
import classNames from 'classnames';

const meterDefaultStyle = css({
    display: 'inline-block',
    width: '5em',
    height: '1em',

    '& > div': {
        backgroundColor: 'lime',
    },
});

function Meter ({ min, max, value, className, ...props }) {
    className = classNames(meterDefaultStyle, className);
    const valueBarStyle = {
        width: `${100 / (max - min) * (value - min)}%`,
        height: '100%',
    };

    return (
        <div className={className} { ...props }>
            <div style={valueBarStyle} />
        </div>
    );
}

export default Meter;
