namespace AssetManager
{
    partial class ModelImporter
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.selectLod0 = new System.Windows.Forms.Button();
            this.pathToLod0 = new System.Windows.Forms.TextBox();
            this.pathToLod1 = new System.Windows.Forms.TextBox();
            this.selectLod1 = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.modelName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.importModel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 48);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(96, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "LOD0 (High Detail)";
            // 
            // selectLod0
            // 
            this.selectLod0.Location = new System.Drawing.Point(267, 62);
            this.selectLod0.Name = "selectLod0";
            this.selectLod0.Size = new System.Drawing.Size(75, 23);
            this.selectLod0.TabIndex = 1;
            this.selectLod0.Text = "Load...";
            this.selectLod0.UseVisualStyleBackColor = true;
            this.selectLod0.Click += new System.EventHandler(this.selectLod0_Click);
            // 
            // pathToLod0
            // 
            this.pathToLod0.Location = new System.Drawing.Point(15, 64);
            this.pathToLod0.Name = "pathToLod0";
            this.pathToLod0.ReadOnly = true;
            this.pathToLod0.Size = new System.Drawing.Size(246, 20);
            this.pathToLod0.TabIndex = 2;
            // 
            // pathToLod1
            // 
            this.pathToLod1.Location = new System.Drawing.Point(15, 103);
            this.pathToLod1.Name = "pathToLod1";
            this.pathToLod1.ReadOnly = true;
            this.pathToLod1.Size = new System.Drawing.Size(246, 20);
            this.pathToLod1.TabIndex = 5;
            // 
            // selectLod1
            // 
            this.selectLod1.Location = new System.Drawing.Point(267, 101);
            this.selectLod1.Name = "selectLod1";
            this.selectLod1.Size = new System.Drawing.Size(75, 23);
            this.selectLod1.TabIndex = 4;
            this.selectLod1.Text = "Load...";
            this.selectLod1.UseVisualStyleBackColor = true;
            this.selectLod1.Click += new System.EventHandler(this.selectLod1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 87);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(94, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "LOD1 (Low Detail)";
            // 
            // modelName
            // 
            this.modelName.Location = new System.Drawing.Point(15, 25);
            this.modelName.Name = "modelName";
            this.modelName.Size = new System.Drawing.Size(327, 20);
            this.modelName.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(67, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Model Name";
            // 
            // importModel
            // 
            this.importModel.Location = new System.Drawing.Point(15, 140);
            this.importModel.Name = "importModel";
            this.importModel.Size = new System.Drawing.Size(327, 23);
            this.importModel.TabIndex = 9;
            this.importModel.Text = "Import";
            this.importModel.UseVisualStyleBackColor = true;
            this.importModel.Click += new System.EventHandler(this.importModel_Click);
            // 
            // ModelImporter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(359, 173);
            this.Controls.Add(this.importModel);
            this.Controls.Add(this.modelName);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.pathToLod1);
            this.Controls.Add(this.selectLod1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.pathToLod0);
            this.Controls.Add(this.selectLod0);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "ModelImporter";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Model Importer";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button selectLod0;
        private System.Windows.Forms.TextBox pathToLod0;
        private System.Windows.Forms.TextBox pathToLod1;
        private System.Windows.Forms.Button selectLod1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox modelName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button importModel;
    }
}