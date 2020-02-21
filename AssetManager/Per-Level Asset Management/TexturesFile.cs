using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
    /* Level texture data bank */
    class TexturesFile
    {
        private static List<Texture> allFiles = new List<Texture>();
        private static int CURRENT_VERSION = 1;
        public static int Version { get { return CURRENT_VERSION; } }

        /* Add/remove textures from the in-memory archive */
        public static int AddFile(Texture _file)
        {
            for (int x = 0; x < allFiles.Count; x++)
            {
                if (allFiles[x].textureContent.SequenceEqual(_file.textureContent)) return allFiles[x].textureID;
            }

            int incrementalID = 0;
            bool validID = false;
            while (!validID)
            {
                for (int i = 0; i < allFiles.Count; i++)
                {
                    if (allFiles[i].textureID == incrementalID)
                    {
                        incrementalID++;
                        break;
                    }
                }
                _file.textureID = incrementalID;
                validID = true;
            }
            allFiles.Add(_file);
            return incrementalID;
        }
        public static void AddFilesFromModel(Model _file)
        {
            //If from obj, we have direct paths to textures
            if (_file.FromOBJ)
            {
                for (int i = 0; i < _file.modelParts.Count; i++)
                {
                    if (!File.Exists(_file.modelParts[i].thisMaterial.texturePath)) continue;
                    Texture newTex = new Texture();
                    newTex.textureContent = File.ReadAllBytes(_file.modelParts[i].thisMaterial.texturePath).ToList();
                    AddFile(newTex);
                }
                return;
            }
            //If not, we already be loaded - so this is likely a logic error in our scripting
        }
        public static void RemoveFile(Texture _file)
        {
            allFiles.Remove(_file);
        }

        /* Get textures list */
        public static List<Texture> GetFiles()
        {
            return allFiles;
        }

        /* Load the texture archive */
        public static void Load(string level_path)
        {
            allFiles.Clear();
            if (!File.Exists(level_path + "LEVEL_TEXTURES.BIN")) return;

            BinaryReader reader = new BinaryReader(File.OpenRead(level_path + "LEVEL_TEXTURES.BIN"));
            int fileVersion = reader.ReadInt32();
            int entryCount = reader.ReadInt32();
            for (int i = 0; i < entryCount; i++)
            {
                Texture newTex = new Texture();
                newTex.textureID = reader.ReadInt32();
                reader.BaseStream.Position += 4; //We don't use the offset here
                newTex.textureContent = new List<byte>(reader.ReadInt32());
                allFiles.Add(newTex);
            }
            reader.Close();

            reader = new BinaryReader(File.OpenRead(level_path + "LEVEL_TEXTURES.PAK"));
            for (int i = 0; i < entryCount; i++)
            {
                List<byte> texCont = new List<byte>();
                for (int x = 0; x < allFiles[i].textureContent.Capacity; x++)
                {
                    texCont.Add(reader.ReadByte());
                }
                allFiles[i].textureContent = texCont;
            }
            reader.Close();
        }

        /* Save the texture archive */
        public static void Save(string level_path)
        {
            BinaryWriter writer = new BinaryWriter(File.OpenWrite(level_path + "LEVEL_TEXTURES.PAK"));
            writer.BaseStream.SetLength(0);
            List<int> fileOffsets = new List<int>();
            for (int i = 0; i < allFiles.Count; i++)
            {
                fileOffsets.Add((int)writer.BaseStream.Position);
                writer.Write(allFiles[i].textureContent.ToArray());
            }
            writer.Close();

            writer = new BinaryWriter(File.OpenWrite(level_path + "LEVEL_TEXTURES.BIN"));
            writer.BaseStream.SetLength(0);
            writer.Write(CURRENT_VERSION);
            writer.Write(allFiles.Count);
            for (int i = 0; i < allFiles.Count; i++)
            {
                writer.Write(allFiles[i].textureID);
                writer.Write(fileOffsets[i]);
                writer.Write(allFiles[i].textureContent.ToArray().Length);
            }
            writer.Close();
        }
    }
}
