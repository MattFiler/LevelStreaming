using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
    class Texture
    {
        public Texture() { }
        public Texture(List<byte> content)
        {
            textureContent = content;
        }
        public List<byte> textureContent = new List<byte>();
        public int textureID = -1; //Managed by TexturesFile
    }
}
