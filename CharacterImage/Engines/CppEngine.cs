using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CharacterImageX.Engines
{
    class CppEngine : Engine
    {
        [DllImport(@"Engine.dll", EntryPoint = "CreateInstance")]
        extern static IntPtr CreateInstance();

        [DllImport(@"Engine.dll", EntryPoint = "DestroyInstance")]
        extern static bool DestroyInstance(IntPtr instance);

        [DllImport(@"Engine.dll", EntryPoint = "AddElement", CharSet = CharSet.Unicode)]
        extern static bool AddElement(IntPtr instance, char c, IntPtr scan0, int width, int height, int stride);

        [DllImport(@"Engine.dll", EntryPoint = "GetStrings")]
        extern static bool GetStrings(IntPtr instance, IntPtr scan0, int width, int height, int stride, int spacing, int lineHeight, IntPtr[] result);

        [DllImport(@"Engine.dll", EntryPoint = "DrawStrings")]
        extern static bool DrawStrings(IntPtr instance, IntPtr scan0, int width, int height, int stride, int spacing, int lineHeight, IntPtr[] result);

        static List<char[]> CreateBuffer(int lines, int countPerLine)
        {
            List<char[]> buffer = new List<char[]>(lines);
            for (int i = 0; i < lines; i++)
            {
                buffer.Add(new char[countPerLine]);
            }

            return buffer;
        }

        static IntPtr[] CreateBuffer(List<char[]> lines)
        {
            IntPtr[] buffer = new IntPtr[lines.Count];
            for (int i = 0; i < lines.Count; i++)
            {
                buffer[i] = Marshal.UnsafeAddrOfPinnedArrayElement(lines[i], 0);
            }

            return buffer;
        }

        static IntPtr[] CreateBuffer(List<string> lines)
        {
            IntPtr[] buffer = new IntPtr[lines.Count];
            for (int i = 0; i < lines.Count; i++)
            {
                char[] line = lines[i].ToCharArray();
                buffer[i] = Marshal.UnsafeAddrOfPinnedArrayElement(line, 0);
            }

            return buffer;
        }

        static List<string> ReadBuffer(IntPtr[] buffer, int countPerLine)
        {
            List<string> lines = new List<string>(buffer.Length);
            for (int i = 0; i < buffer.Length; i++)
            {
                lines.Add(Marshal.PtrToStringUni(buffer[i], countPerLine));
            }

            return lines;
        }

        public CppEngine(IEnumerable<char> chars, Font font) : base(chars, font)
        {
            Instance = CreateInstance();
            foreach (var e in Elements)
            {
                BitmapData data = e.Value.LockBits(new Rectangle(0, 0, e.Value.Width, e.Value.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
                AddElement(Instance, e.Key, data.Scan0, data.Width, data.Height, data.Stride);
                e.Value.UnlockBits(data);
            }
        }

        ~CppEngine()
        {
            DestroyInstance(Instance);
        }

        public override List<string> GetStrings(Bitmap image, int spacing, int lineHeight)
        {
            int lines = (image.Height - lineHeight) / lineHeight + 1;
            int countPerLine = (image.Width - spacing) / spacing + 1;

            List<char[]> _buffer = CreateBuffer(lines, countPerLine);
            IntPtr[] buffer = CreateBuffer(_buffer);

            BitmapData data = image.LockBits(new Rectangle(0, 0, image.Width, image.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
            {
                GetStrings(Instance, data.Scan0, data.Width, data.Height, data.Stride, spacing, lineHeight, buffer);
            }
            image.UnlockBits(data);

            return ReadBuffer(buffer, countPerLine);
        }

        public override Bitmap DrawStrings(Bitmap image, List<string> strings, int spacing, int lineHeight)
        {
            IntPtr[] buffer = CreateBuffer(strings);
            BitmapData data = image.LockBits(new Rectangle(0, 0, image.Width, image.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
            {
                DrawStrings(Instance, data.Scan0, data.Width, data.Height, data.Stride, spacing, lineHeight, buffer);
            }
            image.UnlockBits(data);

            return image;
        }

        protected IntPtr Instance { get; }
    }
}
