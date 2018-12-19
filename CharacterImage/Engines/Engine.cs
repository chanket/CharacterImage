using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CharacterImageX.Engines
{
    abstract class Engine
    {
        protected Font Font { get; }

        protected Dictionary<char, Bitmap> Elements { get; }

        protected Bitmap DrawCharacter(Font font, char c)
        {
            int size = (int)Math.Ceiling(font.SizeInPoints) * 2;
            Bitmap bmp = new Bitmap(size, size);
            Graphics g = Graphics.FromImage(bmp);
            g.FillRectangle(Brushes.White, new Rectangle(0, 0, size, size));
            g.DrawString(c.ToString(), font, Brushes.Black, 0, 0);

            return bmp;
        }

        public Engine(IEnumerable<char> chars, Font font)
        {
            Font = font;

            //Draw Elements
            Elements = new Dictionary<char, Bitmap>();
            foreach (char c in chars)
            {
                Elements.Add(c, DrawCharacter(font, c));
            }
        }

        public abstract List<string> GetStrings(Bitmap image, int spacing, int lineHeight);

        public abstract Bitmap DrawStrings(Bitmap image, List<string> strings, int spacing, int lineHeight);
    }
}
