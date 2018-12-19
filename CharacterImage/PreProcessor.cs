using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CharacterImageX
{
    static class PreProcessor
    {
        [DllImport(@"Engine.dll", EntryPoint = "ScaleColor")]
        extern static void ScaleColor(IntPtr scan0, int width, int height, int stride, double scale);

        [DllImport(@"Engine.dll", EntryPoint = "Resize")]
        extern static void Resize(IntPtr scan0Raw, int widthRaw, int heightRaw, int strideRaw, IntPtr scan0, int width, int height, int stride);

        public static void ScaleColor(BitmapData data, double scale)
        {
            ScaleColor(data.Scan0, data.Width, data.Height, data.Stride, scale);
        }

        public static Bitmap Threshold(Bitmap raw, double threshold)
        {
            Bitmap image = new Bitmap(raw);

            BitmapData data = image.LockBits(new Rectangle(0, 0, image.Width, image.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
            {
                ScaleColor(data, threshold);
            }
            image.UnlockBits(data);

            return image;
        }

        public static Bitmap Resize(Bitmap raw, double scale)
        {
            Bitmap image = new Bitmap((int)(raw.Width * scale), (int)(raw.Height * scale));
            BitmapData dataRaw = raw.LockBits(new Rectangle(0, 0, raw.Width, raw.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
            BitmapData data = image.LockBits(new Rectangle(0, 0, image.Width, image.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
            {
                Resize(dataRaw.Scan0, dataRaw.Width, dataRaw.Height, dataRaw.Stride, data.Scan0, data.Width, data.Height, data.Stride);
            }
            image.UnlockBits(data);
            raw.UnlockBits(dataRaw);
            return image;
        }
    }
}
