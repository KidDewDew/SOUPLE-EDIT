
function movePopupAt(popup,item,x,y) {
    const p0 = item.mapToItem(null,Qt.point(x,y))
    popup.x = p0.x
    popup.y = p0.y
}

function adjustPopupInWindow(popup,window) {
    const p0 = popup.contentItem.mapToItem(null,Qt.point(0,0))
    if(p0.x < 0) popup.x += -p0.x
    else if(p0.x + popup.width > window.width) popup.x -= p0.x+popup.width-window.width
    if(p0.y < 0) popup.y += -p0.y
    else if(p0.y + popup.height > window.height) popup.y -= p0.y+popup.height-window.height
}
