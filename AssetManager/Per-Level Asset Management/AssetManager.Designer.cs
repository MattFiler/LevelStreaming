namespace AssetManager
{
    partial class AssetManager
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
            this.modelList = new System.Windows.Forms.ListBox();
            this.deleteModel = new System.Windows.Forms.Button();
            this.addModel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // modelList
            // 
            this.modelList.FormattingEnabled = true;
            this.modelList.Location = new System.Drawing.Point(12, 12);
            this.modelList.Name = "modelList";
            this.modelList.Size = new System.Drawing.Size(478, 238);
            this.modelList.TabIndex = 8;
            // 
            // deleteModel
            // 
            this.deleteModel.Location = new System.Drawing.Point(496, 216);
            this.deleteModel.Name = "deleteModel";
            this.deleteModel.Size = new System.Drawing.Size(164, 34);
            this.deleteModel.TabIndex = 7;
            this.deleteModel.Text = "Delete Selected Model";
            this.deleteModel.UseVisualStyleBackColor = true;
            this.deleteModel.Click += new System.EventHandler(this.deleteModel_Click);
            // 
            // addModel
            // 
            this.addModel.Location = new System.Drawing.Point(496, 176);
            this.addModel.Name = "addModel";
            this.addModel.Size = new System.Drawing.Size(164, 34);
            this.addModel.TabIndex = 5;
            this.addModel.Text = "Add New Model";
            this.addModel.UseVisualStyleBackColor = true;
            this.addModel.Click += new System.EventHandler(this.addModel_Click);
            // 
            // AssetManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.modelList);
            this.Controls.Add(this.deleteModel);
            this.Controls.Add(this.addModel);
            this.Name = "AssetManager";
            this.Text = "AssetManager";
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.ListBox modelList;
        private System.Windows.Forms.Button deleteModel;
        private System.Windows.Forms.Button addModel;
    }
}