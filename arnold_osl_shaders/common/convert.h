#pragma once
#include <stdosl.h>
#include <oslutil.h>

color XYZToxyY(color in_xyz)
{
   color result;
   float sum = in_xyz[0] + in_xyz[1] + in_xyz[2];
   if (sum > 0.00001)
   {
      result = color(in_xyz[0] / sum, in_xyz[1] / sum, in_xyz[2]);
   }
   else
   {
      result = color(1,1,1);   
   }
   return result;
}

color xyYToXYZ(color in_xyY)
{
   color result;
   result = color(in_xyY[2] * in_xyY[0] / in_xyY[1],in_xyY[2],in_xyY[2] * (1.0 - in_xyY[0] - in_xyY[1]) / in_xyY[1]);
   return result;
}


color convertFromRGB(color inColor,string to_space)
{
   if (to_space == "HSL" || to_space == "HSV")
   {
      float cmax = max(max(inColor[0], inColor[1]), inColor[2]);
      float cmin = min(min(inColor[0], inColor[1]), inColor[2]);
      float chroma = cmax - cmin;
      
      float hue;
      if (chroma == 0.0)
         hue = 0.0;
      else if (cmax == inColor[0])
         hue = (inColor[1] - inColor[2]) / chroma;
      else if (cmax == inColor[1])
         hue = (inColor[2] - inColor[0]) / chroma + 2.0;
      else
         hue = (inColor[0] - inColor[1]) / chroma + 4.0;
      hue *= 1.0 / 6.0;
      if (hue < 0.0)
         hue += 1.0;
     
      if (to_space == "HSL")
      {
         float lightness = (cmax + cmin) * 0.5;
         float saturation;
         if (chroma == 0.0)
            saturation = 0.0;
         else
            saturation = chroma / (1.0 - fabs(2.0 * lightness - 1.0));
         color result = color(hue, saturation, lightness);
         return result;
      }
      else
      {
         // HSV
         float value = cmax;
         float saturation;
         if(chroma == 0.0)
         {
         	saturation =  0.0;
         }
         else
         {
         	saturation = chroma / value;
         }
         color result = color(hue, saturation, value);
         return result;
      }
   }
   else if (to_space == "XYZ" || to_space == "xyY")
   {
      float X = (0.49 * inColor[0] + 0.31 * inColor[1] + 0.2 * inColor[2]) / 0.17697;
      float Y = (0.17697 * inColor[0] + 0.81240 * inColor[1] + 0.01063 * inColor[2]) / 0.17697;
      float Z = (0.0 * inColor[0] + 0.01 * inColor[1] + 0.99 * inColor[2]) / 0.17697;
      color result;
      if (to_space == "XYZ")
         result = color(X, Y, Z);
      else // xyY
      {
         float sum = X + Y + Z;
         if (sum > 0.00001)
            result = color(X / sum, Y / sum, Y);
         else
            result = color(1,1,1);
      }
      return result;
   }
   
   // was RGB already (or unknown color space)
   return inColor;
}

color convertToRGB(color inColor,string from_space)
{
   if (from_space == "HSV" || from_space == "HSL")
   {
      float hue6 = inColor[0] * 6.0;
      float hue2 = hue6;
      if (hue6 > 4.0)      
      	hue2 -= 4.0;
      else if (hue6 > 2.0) 
      	hue2 -= 2.0;
      
      float chroma;
      if (from_space == "HSV")
         chroma = inColor[1] * inColor[2];
      else // HSL
         chroma = (1.0 - fabs(2.0 * inColor[2] - 1.0)) * inColor[1];
      
      float component = chroma * (1.0 - fabs(hue2 - 1.0));
      
      color rgb;
      if (hue6 <= 0.0) 
      	rgb = color(1,1,1);
      else if (hue6 < 1) 
      	rgb = color(chroma, component, 0.0);
      else if (hue6 < 2) 
      	rgb = color(component, chroma, 0.0);
      else if (hue6 < 3) 
      	rgb = color(0.0, chroma, component);
      else if (hue6 < 4) 
      	rgb = color(0.0, component, chroma);
      else if (hue6 < 5) 
      	rgb = color(component, 0.0, chroma);
      else if (hue6 <= 6) 
      	rgb = color(chroma, 0.0, component);
      
      float cmin;
      if (from_space == "HSV")
         cmin = inColor[2] - chroma;
      else // HSL
         cmin = inColor[2] - chroma * 0.5;
      rgb += color(cmin);
      return rgb;
   }
   else if (from_space == "XYZ" || from_space == "xyY")
   {
      // for documentation purposes, CIE->RGB needs color system data, and here
      // are some typical bits that are needed:
      //
      // Name               xRed     yRed     xGreen   yGreen   xBlue    yBlue    White point                Gamma
      // -----------------------------------------------------------------------------------------------------------------------
      // "NTSC",            0.67f,   0.33f,   0.21f,   0.71f,   0.14f,   0.08f,   WhitePoint::IlluminantC,   GAMMA_REC601
      // "EBU (PAL/SECAM)", 0.64f,   0.33f,   0.29f,   0.60f,   0.15f,   0.06f,   WhitePoint::IlluminantD65, GAMMA_REC709
      // "SMPTE",           0.630f,  0.340f,  0.310f,  0.595f,  0.155f,  0.070f,  WhitePoint::IlluminantD65, GAMMA_REC709
      // "HDTV",            0.670f,  0.330f,  0.210f,  0.710f,  0.150f,  0.060f,  WhitePoint::IlluminantD65, GAMMA_REC709 (2.35)
      // "sRGB",            0.670f,  0.330f,  0.210f,  0.710f,  0.150f,  0.060f,  WhitePoint::IlluminantD65, 2.2
      // "CIE",             0.7355f, 0.2645f, 0.2658f, 0.7243f, 0.1669f, 0.0085f, WhitePoint::IlluminantE,   GAMMA_REC709
      // "CIE REC 709",     0.64f,   0.33f,   0.30f,   0.60f,   0.15f,   0.06f,   WhitePoint::IlluminantD65, GAMMA_REC709
      //
      // typical white points are as follows:
      //
      // Name          x            y              Description
      // -----------------------------------------------------------------------------
      // IlluminantA   0.44757f,    0.40745f    Incandescent tungsten
      // IlluminantB   0.34842f,    0.35161f    Obsolete, direct sunlight at noon
      // IlluminantC   0.31006f,    0.31616f    Obsolete, north sky daylight
      // IlluminantD50 0.34567f,    0.35850f    Some print, cameras
      // IlluminantD55 0.33242f,    0.34743f    Some print, cameras
      // IlluminantD65 0.31271f,    0.32902f    For EBU and SMPTE, HDTV, sRGB
      // IlluminantD75 0.29902f,    0.31485f    ???
      // IlluminantE   0.33333333f, 0.33333333f CIE equal-energy illuminant
      // Illuminant93K 0.28480f,    0.29320f    High-efficiency blue phosphor monitors
      // IlluminantF2  0.37207f,    0.37512f    Cool white flourescent (CWF)
      // IlluminantF7  0.31285f,    0.32918f    Broad-band daylight flourescent
      // IlluminantF11 0.38054f,    0.37691f    Narrow-band white flourescent
      
      // we use the CIE equal-energy color space, as it is the most generic
      
      float xr, yr, zr, xg, yg, zg, xb, yb, zb;
      float xw, yw, zw;
      float rx, ry, rz, gx, gy, gz, bx, by, bz;
      float rw, gw, bw;
      
      float xc, yc, zc;
      if (from_space == "XYZ")
      {
         xc = inColor[0];
         yc = inColor[1];
         zc = inColor[2];
      }
      else
      {
         color xyz = xyYToXYZ(inColor);
         xc = xyz[0];
         yc = xyz[1];
         zc = xyz[2];
      }

      // these come from the tables above, using CIE equal-energy color space
      
      xr = 0.7355; yr = 0.2654; zr = 1.0 - (xr + yr);
      xg = 0.2658; yg = 0.7243; zg = 1.0 - (xg + yg);
      xb = 0.1669; yb = 0.0085; zb = 1.0 - (xb + yb);
      
      xw = 1.0 / 3.0; yw = 1.0 / 3.0; zw = 1.0 - (xw + yw);

      // xyz -> rgb matrix, before scaling to white

      rx = (yg * zb) - (yb * zg);  ry = (xb * zg) - (xg * zb);  rz = (xg * yb) - (xb * yg);
      gx = (yb * zr) - (yr * zb);  gy = (xr * zb) - (xb * zr);  gz = (xb * yr) - (xr * yb);
      bx = (yr * zg) - (yg * zr);  by = (xg * zr) - (xr * zg);  bz = (xr * yg) - (xg * yr);

      // white scaling factors; dividing by yw scales the white luminance to unity, as conventional

      rw = ((rx * xw) + (ry * yw) + (rz * zw)) / yw;
      gw = ((gx * xw) + (gy * yw) + (gz * zw)) / yw;
      bw = ((bx * xw) + (by * yw) + (bz * zw)) / yw;

      // xyz -> rgb matrix, correctly scaled to white

      rx = rx / rw;  ry = ry / rw;  rz = rz / rw;
      gx = gx / gw;  gy = gy / gw;  gz = gz / gw;
      bx = bx / bw;  by = by / bw;  bz = bz / bw;

      // rgb of the desired point

      float r = (rx * xc) + (ry * yc) + (rz * zc);
      float g = (gx * xc) + (gy * yc) + (gz * zc);
      float b = (bx * xc) + (by * yc) + (bz * zc);
      
      color result = color(r, g, b);
      return result;
   }
   
   // was RGB already (or unknown color space)
   return inColor;
}
