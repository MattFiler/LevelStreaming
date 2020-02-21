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
    public partial class ModelImporter : Form
    {
        public ModelImporter()
        {
            InitializeComponent();
        }

        /* Select LOD 0/1 */
        private void selectLod0_Click(object sender, EventArgs e)
        {
            OpenFileDialog modelSelect = new OpenFileDialog();
            modelSelect.Filter = "OBJ Model File|*.obj";
            if (modelSelect.ShowDialog() != DialogResult.OK) return;
            pathToLod0.Text = modelSelect.FileName;
            if (modelName.Text == "") modelName.Text = Path.GetFileNameWithoutExtension(pathToLod0.Text).ToUpper().Replace(" ", "_").Trim();
        }
        private void selectLod1_Click(object sender, EventArgs e)
        {
            OpenFileDialog modelSelect = new OpenFileDialog();
            modelSelect.Filter = "OBJ Model File|*.obj";
            if (modelSelect.ShowDialog() != DialogResult.OK) return;
            pathToLod1.Text = modelSelect.FileName;
            if (modelName.Text == "") modelName.Text = Path.GetFileNameWithoutExtension(pathToLod1.Text).ToUpper().Replace(" ", "_").Trim();
        }

        /* Import */
        private void importModel_Click(object sender, EventArgs e)
        {
            if (modelName.Text == "")
            {
                MessageBox.Show("Name cannot be blank!", "Invalid input", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (pathToLod0.Text == "" || pathToLod1.Text == "")
            {
                MessageBox.Show("Models must be selected for both LOD 0 & 1!", "Invalid input", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            for (int i = 0; i < ModelsFile.GetFiles().Count; i++)
            {
                if (ModelsFile.GetFiles()[i].modelName == modelName.Text)
                {
                    MessageBox.Show("Model name must be unique!", "Invalid input", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            Model newModelLOD0 = new Model(modelName.Text, LevelOfDetail.HIGH);
            if (!newModelLOD0.LoadFromOBJ(pathToLod0.Text))
            {
                MessageBox.Show("Model selected for LOD0 is invalid!\nModel must be triangulated and contain UV data.", "Invalid LOD0", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            Model newModelLOD1 = new Model(modelName.Text, LevelOfDetail.LOW);
            if (!newModelLOD1.LoadFromOBJ(pathToLod1.Text))
            {
                MessageBox.Show("Model selected for LOD1 is invalid!\nModel must be triangulated and contain UV data.", "Invalid LOD0", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            ModelsFile.AddFile(newModelLOD0);
            TexturesFile.AddFilesFromModel(newModelLOD0);
            ModelsFile.AddFile(newModelLOD1);
            TexturesFile.AddFilesFromModel(newModelLOD1);

            MessageBox.Show("Model successfully imported.", "Import complete", MessageBoxButtons.OK, MessageBoxIcon.Information);
            this.Close();
        }
    }
}
