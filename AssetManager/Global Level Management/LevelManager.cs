using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AssetManager
{
    public partial class LevelManager : Form
    {
        private List<Level> allLevels = new List<Level>();

        public LevelManager()
        {
            InitializeComponent();
        }

        /* Reload GUI List */
        private void ReloadList(object sender, EventArgs e)
        {
            ReloadList();
        }
        private void ReloadList()
        {
            levelList.Items.Clear();
            LevelFile.LoadData();
            foreach (Level level in LevelFile.GetData())
            {
                levelList.Items.Add(level);
            }
        }

        /* Create a new level */
        private void addLevel_Click(object sender, EventArgs e)
        {
            CreateLevel newLevel = new CreateLevel();
            newLevel.FormClosed += new FormClosedEventHandler(ReloadList);
            newLevel.Show();
        }

        /* Manage assets for a selected existing level */
        private void manageLevel_Click(object sender, EventArgs e)
        {

        }

        /* Delete a selected level */
        private void deleteLevel_Click(object sender, EventArgs e)
        {

        }
    }
}
