#pragma once
#include <stdosl.h>
#include <oslutil.h>

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

color lerp(color a, color b, float t) {
    color r;
    r[0] = lerp(a[0], b[0], t);
    r[1] = lerp(a[1], b[1], t);
    r[2] = lerp(a[2], b[2], t);
    return r;
}

color overlay(color l1,color l2) 
{
    color result;
    for (int i = 0; i < 3; ++i) {
        l1[i] < 0.5
            ? result[i] = 2.0 * l1[i] * l2[i]
            : result[i] = (1.0 - 2.0 * (1.0 - l1[i]) * (1.0 - l2[i]));
    }
    return result;
}

color softlight(color l1, color l2) {
    color result;
    for (int i = 0; i < 3; ++i) {
        if (l2[i] < 0.5)
            result[i] =
                2.0 * l1[i] * l2[i] + sqrt(l1[i]) * (1.0 - 2.0 * l2[i]);
        else
            result[i] = sqrt(l1[i]) * (2.0 * l2[i] - 1.0) +
                        2.0 * l1[i] * (1.0 - l2[i]);
    }

    return result;
}

float colordodgef(float l1, float l2, int clamp_result) {
    float result;
    if (l2 == 1.0) 
    {
        result = l2;
    } 
    else 
    {
        float inre = l1 / (1.0 - l2);
        if (clamp_result)
            inre = min(inre, 1.0);
        result = inre;
    }
    return result;
}
color colordodge(color l1, color l2, int clamp_result) {
    color result;
    for (int i = 0; i < 3; ++i) {
        result[i] = colordodgef(l1[i], l2[i], clamp_result);
    }
    return result;
}

float colorburnf(float l1, float l2) {
    if (l2 == 0.0)
        return l2;
    else
        return max(1.0 - (1.0 - l1) / l2, 0.0);
}

color colorburn(color l1, color l2) {
    color result;
    for (int i = 0; i < 3; ++i) {
        result[i] = colorburnf(l1[i], l2[i]);
    }

    return result;
}


color linearlight(color l1, color l2, int clamp_result) {
    color result;
    for (int i = 0; i < 3; ++i) {
        if (l2[i] < 0.5)
            result[i] = l1[i] + 2.0 * l2[i] - 1.0;
        else
            result[i] = l1[i] + 2.0 * (l2[i] - 0.5);
    }

    if (clamp_result)
        result = clamp(result, color(0,0,0), color(1,1,1));

    return result;
}

color vividlight(color l1, color l2, int clamp_result) {
    color result;
    for (int i = 0; i < 3; ++i) {
        if (l2[i] < 0.5) 
            result[i] = colorburnf(l1[i], 2.0 * l2[i]);
        else
            result[i] = colordodgef(l1[i], 2.0 * (l2[i] - 0.5), clamp_result);
    }

    return result;
}

color pinlight(color l1, color l2) {
    color result;
    for (int i = 0; i < 3; ++i) {
        if (l2[i] < 0.5)
            result[i] = min(l1[i], 2.0 * l2[i]);
        else
            result[i] = max(l1[i], 2.0 * (l2[i] - 0.5));
    }

    return result;
}

color hardmix(color l1, color l2, int clamp_result) {
    color result = vividlight(l1, l2, clamp_result);
    for (int i = 0; i < 3; ++i) {
        result[i] < 0.5 ? result[i] = 0.0 : result[i] = 1.0;
    }

    return result;
}

color reflects(color l1, color l2, int clamp_result) {
    color result;
    for (int i = 0; i < 3; ++i) {
        if (l2[i] == 1.0)
            result[i] = l2[i];
        else
            result[i] = sqrt(l1[i]) / (1.0 - l2[i]);
    }

    if (clamp_result)
        result = clamp(result, color(0,0,0), color(1,1,1));

    return result;
}

float al_luminance(color c) {
    return c[0] * 0.212671 + c[1] * 0.715160 + c[2] * 0.072169;
}

float al_luminance(float f) { return f; }

float al_contrast(float input, float contrastVal, float contrastPivot) {
    if (contrastVal == 1.0)
        return input;
    return (input - contrastPivot) * contrastVal + contrastPivot;
}

color al_contrast(color input, float contrastVal, float contrastPivot) {
    if (contrastVal == 1.0)
        return input;

    return color((input[0]- contrastPivot) * contrastVal + contrastPivot,
                 (input[1] - contrastPivot) * contrastVal + contrastPivot,
                 (input[2] - contrastPivot) * contrastVal + contrastPivot);
}

color al_hsv2rgb(color hsv) 
{
    float h = hsv[0];
    float s = hsv[1];
    float v = hsv[2];

    if (s < 0.0001)
    {
        return color(v, v, v);
    }
    else 
    {
        h = 6.0 * (h - floor(h));
        int hi = (int)h;
        float f = h - hi;
        float p = v * (1.0 - s);
        float q = v * (1.0 - s * f);
        float t = v * (1.0 - s * (1.0 - f));
        if(hi == 0)
            return color(v, t, p);
        else if (hi == 1)
            return color(q, v, p);
        else if (hi == 2)
            return color(p, v, t);
        else if (hi == 3)
            return color(p, q, v);
        else if (hi == 4)
            return color(t, p, v);
        else
            return color(v, p, q);
    }
}


color al_rgb2hsv(color rgb) {
    float r = rgb[0], g = rgb[1], b = rgb[2];
    float mincomp = min(r, min(g, b));
    float maxcomp = max(r, max(g, b));
    float delta = maxcomp - mincomp; // chroma
    float h, s, v;
    v = maxcomp;
    if (maxcomp > 0)
        s = delta / maxcomp;
    else
        s = 0;
    if (s <= 0)
        h = 0;
    else {
        if (r >= maxcomp)
            h = (g - b) / delta;
        else if (g >= maxcomp)
            h = 2 + (b - r) / delta;
        else
            h = 4 + (r - g) / delta;
        h /= 6;
        if (h < 0)
            h += 1;
    }
    return color(h, s, v);
}

float al_bias(float f, float b) 
{
    if (b > 0.0)
        return pow(f, log(b) / log(0.5));
    else
        return 0.0;
}

float al_biasandgain(float f, float b, float g) 
{
    if (f < 0)
        return f;

    if (b != 0.5)
        f = al_bias(f, b);

    if (g != 0.5) {
        if (f < 0.5)
            f = 0.5 * al_bias(2.0 * f, 1.0 - g);
        else
            f = 1.0 - al_bias(2.0 - 2.0 * f, 1.0 - g) * 0.5;
    }
    return f;
}
