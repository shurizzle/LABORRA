import React from 'react/cjs/react.production.min.js';

function ChildrenWithSeparator({ children, separator, ...props }) {
    children = React.Children.toArray(children).filter(x => x != null);

    if (children.length === 0) {
        return null;
    } else if (children.length === 1) {
        return children[0];
    }

    const res = [];
    children.forEach((element, i) => {
        if (i !== 0) {
            res.push(React.cloneElement(separator, { key: res.length }));
        }
        res.push(React.cloneElement(element, { key: res.length }));
    });

    return React.createElement(React.Fragment, props, res);
}

export default ChildrenWithSeparator;
