#include <float>


enum VirtualCanvas {
	Float:canvasMinX,
	Float:canvasMaxX,
	Float:canvasMinY,
	Float:canvasMaxY,
	canvasResX,
	canvasResY
}

// CreateVirtualCanvas is a helper for constructing virtual canvas objects
stock CreateVirtualCanvas(Float:minX, Float:maxX, Float:minY, Float:maxY, resX, resY, output[VirtualCanvas]) {
	if(resX < 0 || resY < 0) {
		return 1;
	}

	output[canvasMinX] = minX;
	output[canvasMaxX] = maxX;
	output[canvasMinY] = minY;
	output[canvasMaxY] = maxY;
	output[canvasResX] = resX;
	output[canvasResY] = resY;

	return 0;
}

// GetVirtualCanvasPos scales the given integer position for the given canvas, returns 0 on success
stock GetVirtualCanvasPos(canvas[VirtualCanvas], posX, posY, &Float:resultX, &Float:resultY) {
	if(posX < 0 || posY < 0 || posX > canvas[canvasResX] || posY > canvas[canvasResY]) {
		return 1;
	}

	resultX = canvas[canvasMinX] + (((canvas[canvasMaxX] - canvas[canvasMinX]) / canvas[canvasResX]) * float(posX));
	resultY = canvas[canvasMinY] + (((canvas[canvasMaxY] - canvas[canvasMinY]) / canvas[canvasResY]) * float(posY));

	return 1;
}
