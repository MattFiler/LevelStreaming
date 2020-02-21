using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
    /* Level model data bank */
    class ModelsFile
    {
        private static List<Model> allFiles = new List<Model>();

        /* Add/remove models from the in-memory archive */
        public static void AddFile(Model _file)
        {
            allFiles.Add(_file);
        }
        public static void RemoveFile(Model _file)
        {
            allFiles.Remove(_file);
        }

        /* Get model list */
        public static List<Model> GetFiles()
        {
            return allFiles;
        }

        /* Load the model archive */
        public static void Load()
        {
            allFiles.Clear();
            if (!File.Exists("LEVEL_MODELS.BIN")) return;

            BinaryReader reader = new BinaryReader(File.OpenRead("LEVEL_MODELS.BIN"));
            int fileVersion = reader.ReadInt32();
            int entryCount = reader.ReadInt32();
            List<int> vertCountComp = new List<int>();
            for (int i = 0; i < entryCount; i++)
            {
                Model newModel = new Model((LevelOfDetail)reader.ReadInt16());
                newModel.modelName = reader.ReadString();
                reader.BaseStream.Position += 8; //We shouldn't need the offset/length here
                int numModelParts = reader.ReadInt32();
                for (int x = 0; x < numModelParts; x++)
                {
                    newModel.modelParts.Add(new ModelPart());
                }
                for (int x = 0; x < newModel.modelParts.Count; x++)
                {
                    int numVertsInPart = reader.ReadInt32();
                    for (int y = 0; y < numVertsInPart; y++)
                    {
                        newModel.modelParts[x].compVertices.Add(new SimpleVertex());
                    }
                }
                allFiles.Add(newModel);
            }
            reader.Close();

            reader = new BinaryReader(File.OpenRead("LEVEL_MODELS.PAK"));
            for (int i = 0; i < entryCount; i++)
            {
                for (int x = 0; x < allFiles[i].modelParts.Count; x++)
                {
                    allFiles[i].modelParts[x].thisMaterial.colourTint.r = reader.ReadSingle();
                    allFiles[i].modelParts[x].thisMaterial.colourTint.g = reader.ReadSingle();
                    allFiles[i].modelParts[x].thisMaterial.colourTint.b = reader.ReadSingle();
                    allFiles[i].modelParts[x].thisMaterial.colourTint.a = reader.ReadSingle();
                    allFiles[i].modelParts[x].thisMaterial.textureID = reader.ReadInt32();

                    for (int y = 0; y < allFiles[i].modelParts[x].compVertices.Count; y++)
                    {
                        allFiles[i].modelParts[x].compIndices.Add(reader.ReadInt32());

                        allFiles[i].modelParts[x].compVertices[y].Pos.x = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Pos.y = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Pos.z = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Tex.x = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Tex.y = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Normal.x = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Normal.y = reader.ReadSingle();
                        allFiles[i].modelParts[x].compVertices[y].Normal.z = reader.ReadSingle();
                    }
                }
            }
            reader.Close();
        }

        /* Save the model archive */
        public static void Save()
        {
            BinaryWriter writer = new BinaryWriter(File.OpenWrite("LEVEL_MODELS.PAK"));
            writer.BaseStream.SetLength(0);
            List<int> fileOffsets = new List<int>();
            for (int i = 0; i < allFiles.Count; i++)
            {
                fileOffsets.Add((int)writer.BaseStream.Position);
                for (int x = 0; x < allFiles[i].modelParts.Count; x++)
                {
                    writer.Write(allFiles[i].modelParts[x].thisMaterial.colourTint.r);
                    writer.Write(allFiles[i].modelParts[x].thisMaterial.colourTint.g);
                    writer.Write(allFiles[i].modelParts[x].thisMaterial.colourTint.b);
                    writer.Write(allFiles[i].modelParts[x].thisMaterial.colourTint.a);
                    writer.Write(allFiles[i].modelParts[x].thisMaterial.textureID);

                    for (int y = 0; y < allFiles[i].modelParts[x].compVertices.Count; y++)
                    {
                        writer.Write(allFiles[i].modelParts[x].compIndices[y]);

                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Pos.x);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Pos.y);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Pos.z);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Tex.x);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Tex.y);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Normal.x);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Normal.y);
                        writer.Write(allFiles[i].modelParts[x].compVertices[y].Normal.z);
                    }
                }
            }
            fileOffsets.Add((int)writer.BaseStream.Position);
            writer.Close();

            writer = new BinaryWriter(File.OpenWrite("LEVEL_MODELS.BIN"));
            writer.BaseStream.SetLength(0);
            writer.Write(1);
            writer.Write(allFiles.Count);
            for (int i = 0; i < allFiles.Count; i++)
            {
                writer.Write((Int16)allFiles[i].modelLOD);
                writer.Write(allFiles[i].modelName);
                writer.Write(fileOffsets[i]);
                writer.Write(fileOffsets[i+1] - fileOffsets[i]);
                writer.Write(allFiles[i].modelParts.Count);
                for (int x = 0; x < allFiles[i].modelParts.Count; x++)
                {
                    writer.Write(allFiles[i].modelParts[x].compVertices.Count);
                }
            }
            writer.Close();
        }
    }
}
