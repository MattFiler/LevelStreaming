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
        public AssetManager()
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
            modelList.Items.Clear();
            ModelsFile.Load();
            TexturesFile.Load();
            foreach (Model model in ModelsFile.GetFiles())
            {
                modelList.Items.Add(model.modelName);
            }
        }

        private void addModel_Click(object sender, EventArgs e)
        {
            OpenFileDialog modelSelect = new OpenFileDialog();
            modelSelect.Filter = "OBJ Model File|*.obj";
            if (modelSelect.ShowDialog() != DialogResult.OK) return;
            Model newModel = new Model(LevelOfDetail.HIGH);
            newModel.LoadFromOBJ(modelSelect.FileName);
            ModelsFile.AddFile(newModel);
            TexturesFile.AddFilesFromModel(newModel);
            ModelsFile.Save();
            TexturesFile.Save();
            ReloadList();
        }

        private void deleteModel_Click(object sender, EventArgs e)
        {
            if (modelList.SelectedIndex == -1) return;
            ModelsFile.RemoveFile(ModelsFile.GetFiles()[modelList.SelectedIndex]);
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
            ModelsFile.Save();
            TexturesFile.Save();
            ReloadList();
        }
    }
}
