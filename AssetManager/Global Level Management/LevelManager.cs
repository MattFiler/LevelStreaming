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
    public partial class LevelManager : Form
    {
        public LevelManager()
        {
            InitializeComponent();
            ReloadList();
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
                levelList.Items.Add(level.levelName);
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
            if (levelList.SelectedIndex == -1) return;

            AssetManager assetEditor = new AssetManager(LevelFile.GetData()[levelList.SelectedIndex].levelPath);
            assetEditor.FormClosed += new FormClosedEventHandler(ReloadList);
            assetEditor.Show();
        }

        /* Delete a selected level */
        private void deleteLevel_Click(object sender, EventArgs e)
        {
            if (levelList.SelectedIndex == -1) return;
            DialogResult shouldDo = MessageBox.Show("Are you sure you wish to delete this level?", "Confirmation...", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (shouldDo != DialogResult.Yes) return;

            Level toRemove = LevelFile.GetData()[levelList.SelectedIndex];
            LevelFile.GetData().Remove(toRemove);
            if (toRemove.levelPath.Substring(0, 5) == "DATA/") toRemove.levelPath = toRemove.levelPath.Substring(5);
            Directory.Delete(toRemove.levelPath, true);

            MessageBox.Show("Successfully deleted!", "Deleted", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
