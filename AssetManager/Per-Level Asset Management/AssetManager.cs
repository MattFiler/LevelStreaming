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
    public partial class AssetManager : Form
    {
        private string levelPath;
        public AssetManager(string level_path)
        {
            levelPath = level_path;
            if (levelPath.Substring(0, 5) == "DATA/")
            {
                levelPath = levelPath.Substring(5);
            }
            InitializeComponent();
            ReloadList();
        }

        /* Reload GUI List */
        private void SaveAndReloadList(object sender, EventArgs e)
        {
            ModelsFile.Save(levelPath);
            TexturesFile.Save(levelPath);
            ReloadList();
        }
        private void ReloadList(object sender, EventArgs e)
        {
            ReloadList();
        }
        private void ReloadList()
        {
            modelList.Items.Clear();
            ModelsFile.Load(levelPath);
            TexturesFile.Load(levelPath);
            foreach (Model model in ModelsFile.GetFiles())
            {
                if (!modelList.Items.Contains(model.modelName)) modelList.Items.Add(model.modelName); //We share names between LOD0&1, so don't duplicate
            }
        }

        /* Add a model to the level */
        private void addModel_Click(object sender, EventArgs e)
        {
            ModelImporter modelImporter = new ModelImporter();
            modelImporter.FormClosed += new FormClosedEventHandler(SaveAndReloadList);
            modelImporter.Show();
        }

        /* Delete a model from the level */
        private void deleteModel_Click(object sender, EventArgs e)
        {
            if (modelList.SelectedIndex == -1) return;
            
            for (int i = 0; i < ModelsFile.GetFiles().Count; i++)
            {
                if (ModelsFile.GetFiles()[i].modelName == modelList.Items[modelList.SelectedIndex].ToString())
                {
                    ModelsFile.RemoveFile(ModelsFile.GetFiles()[i]);
                }
            }

            Texture[] prevTexs = new Texture[TexturesFile.GetFiles().Count];
            TexturesFile.GetFiles().CopyTo(prevTexs);
            TexturesFile.GetFiles().Clear();
            for (int i = 0; i < ModelsFile.GetFiles().Count; i++) {
                for (int x = 0; x < ModelsFile.GetFiles()[i].modelParts.Count; x++)
                {
                    for (int y = 0; y < prevTexs.Length; y++)
                    {
                        if (prevTexs[y].textureID == ModelsFile.GetFiles()[i].modelParts[x].thisMaterial.textureID)
                        {
                            TexturesFile.AddFile(prevTexs[y]);
                            break;
                        }
                    }
                }
            }

            ModelsFile.Save(levelPath);
            TexturesFile.Save(levelPath);

            ReloadList();
        }
    }
}
