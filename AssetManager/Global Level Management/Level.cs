using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
    /* Level types */
    public enum LevelType
    {
        FE_LEVEL,
        STD_LEVEL
    }

    /* A level */
    class Level
    {
        public Level(string _name, string _path, LevelType _type)
        {
            levelName = _name;
            levelPath = _path;
            levelType = _type;
        }
        public string levelName;
        public string levelPath;
        public LevelType levelType;
    }
}
