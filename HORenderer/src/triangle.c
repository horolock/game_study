#include "display.h"
#include "triangle.h"
#include "swap.h"

vec3_t barycentricWeights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);
    vec2_t ap = vec2_sub(p, a);

    /**
     * Area of the full paralleleogram (triangle ABC) using cross product
     * | ac.x | \/ | ab.x |
     * | ac.y | /\ | ab.y |
     * ac.x * ab.y - ac.y * ab.x
     */
    float areaParallelogramABC = (ac.x * ab.y) - (ac.y * ab.x);

    /**
     * Alpha = area of parallelogram PBC over the area of the full parallelogram ABC
     * || PC x PB ||
     * -------------
     * || AC x AB ||
     * 
     * Beta = area of parallelogram APC over the area of the full parallelogram ABC
     * || AC x AP ||
     * -------------
     * || AC x AB ||
     * 
     * Gamma can be easily found since barycentric coordinates alwasy add up to 1.0
     * gamma = 1.0 - alpha - beta
     */
    float alpha = ((pc.x * pb.y) - (pc.y * pb.x)) / areaParallelogramABC;
    float beta = ((ac.x * ap.y) - (ac.y * ap.x)) / areaParallelogramABC;
    float gamma = 1.0 - alpha - beta;

    vec3_t weights = {alpha, beta, gamma};

    return weights;
}

void drawTrianglePixel(Point p, uint32_t color, vec4_t pointA, vec4_t pointB, vec4_t pointC) {
    /* Create three vec2 to find the interpolation */
    vec2_t targetPoint = {p.x, p.y};
    vec2_t a = vec2_from_vec4(pointA);
    vec2_t b = vec2_from_vec4(pointB);
    vec2_t c = vec2_from_vec4(pointC);

    /* Calculate barycentric coordinates of 'targetPoint' which is inside the triangle */
    vec3_t weights = barycentricWeights(a, b, c, targetPoint);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    /* Interpolate the value of 1/w for the current pixel */
    float interpolatedReciprocalW = (1 / pointA.w) * alpha + (1 / pointB.w) * beta + (1 / pointC.w) * gamma;

    /* Adjust 1/w so the pixels that are closer to the camera have smaller values */
    interpolatedReciprocalW = 1.0 - interpolatedReciprocalW;

    /* Only draw the pixel if the depth value is less than the one previously stored in the z-buffer */
    if (interpolatedReciprocalW < zBuffer[(windowWidth * p.y) + p.x]) {
        drawPixel(p, color);

        /* Update the z-buffer value with the 1/w of this current pixel */
        zBuffer[(windowWidth * p.y) + p.x] = interpolatedReciprocalW;
    }
}

void drawTriangleTexel(Point p, uint32_t* texture, vec4_t pointA, vec4_t pointB, vec4_t pointC, tex2_t aUV, tex2_t bUV, tex2_t cUV) {
    vec2_t targetPoint = {p.x, p.y};
    vec2_t a = vec2_from_vec4(pointA);
    vec2_t b = vec2_from_vec4(pointB);
    vec2_t c = vec2_from_vec4(pointC);

    /* Calculate the barycentric coordinates of our point 'p' inside the triangle */
    vec3_t weights = barycentricWeights(a, b, c, targetPoint);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    /* Variables to store the interpolated values of U, V, and also 1/w for the current pixel */
    float interpolatedU;
    float interpolatedV;
    float interpolatedReciprocalW;

    /* Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w */
    interpolatedU = (aUV.u / pointA.w) * alpha + (bUV.u / pointB.w) * beta + (cUV.u / pointC.w) * gamma;
    interpolatedV = (aUV.v / pointA.w) * alpha + (bUV.v / pointB.w) * beta + (cUV.v / pointC.w) * gamma;

    interpolatedReciprocalW = (1 / pointA.w) * alpha + (1 / pointB.w) * beta + (1 / pointC.w) * gamma;

    /* Divide back both interpolated values by 1/w */
    interpolatedU /= interpolatedReciprocalW;
    interpolatedV /= interpolatedReciprocalW;

    /* Map the UV coordinate to the full texture width and height */
    int texX = abs((int)(interpolatedU * texture_width)) % texture_width;
    int texY = abs((int)(interpolatedV * texture_height)) % texture_height;

    interpolatedReciprocalW = 1.0 - interpolatedReciprocalW;

    /* Only draw the pixel if the depth value is less thant the one previously stored in the z-buffer */
    if (interpolatedReciprocalW < zBuffer[(windowWidth * p.y) + p.x]) {
        drawPixel(p, texture[(texture_width * texY) + texX]);

        /* Update the z-buffer value with the 1/w of this current pixel */
        zBuffer[(windowWidth * p.y) + p.x] = interpolatedReciprocalW;
    }
}

void drawTriangle(Point p0, Point p1, Point p2, uint32_t color) {
    drawLine(p0, p1, color);
    drawLine(p1, p2, color);
    drawLine(p2, p0, color);
}

void drawFilledTriangle(Point4 p0, Point4 p1, Point4 p2, uint32_t color) {
    /* Sort the vertices by y-coordinate ascending (y0 < y1 < y2) */
    if (p0.y > p1.y) { swapPoint4(&p0, &p1); }
    if (p1.y > p2.y) { swapPoint4(&p1, &p2); }
    if (p0.y > p1.y) { swapPoint4(&p0, &p1); }

    vec4_t pointA = {p0.x, p0.y, p0.z, p0.w};
    vec4_t pointB = {p1.x, p1.y, p1.z, p1.w};
    vec4_t pointC = {p2.x, p2.y, p2.z, p2.w};

    /**
     * Render the upper part of the triangle (Flat-Bottom)
     */
    float invSlope1 = 0;
    float invSlope2 = 0;

    if (p1.y - p0.y != 0) { invSlope1 = (float)(p1.x - p0.x) / abs(p1.y - p0.y); }
    if (p2.y - p0.y != 0) { invSlope2 = (float)(p2.x - p0.x) / abs(p2.y - p0.y); }

    if (p1.y - p0.y != 0) {
        for (int y = p0.y; y < p1.y; y++) {
            int xStart = p1.x + (y - p1.y) * invSlope1;
            int xEnd = p0.x + (y - p0.y) * invSlope2;

            if (xEnd < xStart) { int_swap(&xStart, &xEnd); }

            for (int x = xStart; x < xEnd; x++) {
                Point pixelToDraw = {x, y};
                drawTrianglePixel(pixelToDraw, color, pointA, pointB, pointC);
            }
        }
    }

    /**
     * Render the bottom part of the triangle (Flat-Top)
     */
    invSlope1 = 0;
    invSlope2 = 0;

    if (p2.y - p1.y != 0) { invSlope1 = (float)(p2.x - p1.x) / abs(p2.y - p1.y); }
    if (p2.y - p0.y != 0) { invSlope2 = (float)(p2.x - p0.x) / abs(p2.y - p0.y); }

    if (p2.y - p1.y != 0) {
        for (int y = p1.y; y <= p2.y; y++) {
            int xStart = p1.x + (y - p1.y) * invSlope1;
            int xEnd = p0.x + (y - p0.y) * invSlope2;

            if (xEnd < xStart) { int_swap(&xStart, &xEnd); }
            for (int x = xStart; x < xEnd; x++) {
                Point pixelToDraw = {x, y};
                drawTrianglePixel(pixelToDraw, color, pointA, pointB, pointC);
            }
        }
    }
}

void drawTexturedTriangle(TexturePoint p0, TexturePoint p1, TexturePoint p2, uint32_t* texture) {
    /* Sort the vertices by y-coordinate ascending (y0 < y1 < y2) */
    if (p0.y > p1.y) { swapTexturePoint(&p0, &p1); }
    if (p1.y > p2.y) { swapTexturePoint(&p1, &p2); }
    if (p0.y > p1.y) { swapTexturePoint(&p0, &p1); }

    /* Flip the V component to account for inverted UV-Coordinates (V grows downwards) */
    p0.v = 1.0 - p0.v;
    p1.v = 1.0 - p1.v;
    p2.v = 1.0 - p2.v;

    /* Create vector points and texture coords after we sort the vertices */
    vec4_t pointA = {p0.x, p0.y, p0.z, p0.w};
    vec4_t pointB = {p1.x, p1.y, p1.z, p1.w};
    vec4_t pointC = {p2.x, p2.y, p2.z, p2.w};

    tex2_t aUV = {p0.u, p0.v};
    tex2_t bUV = {p1.u, p1.v};
    tex2_t cUV = {p2.u, p2.v};


    /**
     * Render the upper part of the triangle (Flat-Bottom)
     */
    float invSlope1 = 0;
    float invSlope2 = 0;

    if (p1.y - p0.y != 0) { invSlope1 = (float)(p1.x - p0.x) / abs(p1.y - p0.y); }
    if (p2.y - p0.y != 0) { invSlope2 = (float)(p2.x - p0.x) / abs(p2.y - p0.y); }

    if (p1.y - p0.y != 0) {
        for (int y = p0.y; y <= p1.y; y++) {
            int xStart = p1.x + (y - p1.y) * invSlope1;
            int xEnd = p0.x + (y - p0.y) * invSlope2;

            if (xEnd < xStart) { int_swap(&xStart, &xEnd); }

            for (int x = xStart; x < xEnd; x++) {
                Point pixelToDraw = {x, y};
                drawTriangleTexel(pixelToDraw, texture, pointA, pointB, pointC, aUV, bUV, cUV);
            }
        }
    }

    /**
     * Render the bottom part of the triangle (Flat-Top)
     */
    invSlope1 = 0;
    invSlope2 = 0;

    if (p2.y - p1.y != 0) { invSlope1 = (float)(p2.x - p1.x) / abs(p2.y - p1.y); }
    if (p2.y - p0.y != 0) { invSlope2 = (float)(p2.x - p0.x) / abs(p2.y - p0.y); }

    if (p2.y - p1.y != 0) {
        for (int y = p1.y; y <= p2.y; y++) {
            int xStart = p1.x + (y - p1.y) * invSlope1;
            int xEnd = p0.x + (y - p0.y) * invSlope2;

            if (xEnd < xStart) { int_swap(&xStart, &xEnd); }

            for (int x = xStart; x < xEnd; x++) {
                Point pixelToDraw = {x, y};
                drawTriangleTexel(pixelToDraw, texture, pointA, pointB, pointC, aUV, bUV, cUV);
            }
        }
    }

}