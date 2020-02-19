using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
    /* Level database file management */
    class LevelFile
    {
        private static List<Level> levelList = new List<Level>();
        private static int CURRENT_VERSION = 1;
        public static int Version { get { return CURRENT_VERSION; } }
        private static string PATH_TO_FILE = "LEVELS.LSF";

        /* Load level list */
        public static bool LoadData()
        {
            if (!File.Exists(PATH_TO_FILE)) return false;
            levelList.Clear();
            BinaryReader reader = new BinaryReader(File.OpenRead(PATH_TO_FILE));
            int entryCount = reader.ReadInt32();
            int versionNum = reader.ReadInt32();
            //if (versionNum != CURRENT_VERSION)
            for (int i = 0; i < entryCount; i++)
            {
                levelList.Add(new Level(reader.ReadString(), reader.ReadString(), (LevelType)reader.ReadInt16()));
            }
            reader.Close();
            return true;
        }

        /* Get level list */
        public static List<Level> GetData()
        {
            return levelList;
        }

        /* Save level list */
        public static bool SaveData()
        {
            BinaryWriter writer = new BinaryWriter(File.OpenWrite(PATH_TO_FILE));
            writer.BaseStream.SetLength(0);
            writer.Write(levelList.Count);
            writer.Write(CURRENT_VERSION);
            for (int i = 0; i < levelList.Count; i++)
            {
                writer.Write(levelList[i].levelName);
                writer.Write(levelList[i].levelPath);
                writer.Write((Int16)levelList[i].levelType);
            }
            writer.Close();
            return false;
        }
    }
}
