using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AssetManager
{
    public partial class CreateLevel : Form
    {
        public CreateLevel()
        {
            InitializeComponent();
            levelType.SelectedIndex = 1;
        }

        /* Save the new level */
        private void saveLevel_Click(object sender, EventArgs e)
        {
            if (levelName.Text == "")
            {
                MessageBox.Show("Level must have a name!", "No level name.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            for (int i = 0; i < LevelFile.GetData().Count; i++)
            {
                if (LevelFile.GetData()[i].levelName == levelName.Text)
                {
                    MessageBox.Show("Level name must be unique!", "Level name already exists.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            string pathToEnv = "DATA/ENV/PRODUCTION/" + levelName.Text.Trim().ToUpper().Replace(' ', '_') + "/";
            LevelFile.GetData().Add(new Level(levelName.Text, pathToEnv, (LevelType)levelType.SelectedIndex));
            LevelFile.SaveData();

            Directory.CreateDirectory(pathToEnv);
            File.WriteAllBytes(pathToEnv + "COMMANDS.BIN", Properties.Resources.COMMANDS_BIN);
            File.WriteAllBytes(pathToEnv + "COMMANDS.JSON", Properties.Resources.COMMANDS_JSON);
            Directory.CreateDirectory(pathToEnv + "TEXTURES");

            MessageBox.Show("Saved!", "Complete.", MessageBoxButtons.OK, MessageBoxIcon.Information);
            this.Close();
        }
    }
}
