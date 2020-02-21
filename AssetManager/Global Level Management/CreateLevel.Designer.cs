namespace AssetManager
{
    partial class CreateLevel
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
            this.saveLevel = new System.Windows.Forms.Button();
            this.levelType = new System.Windows.Forms.ComboBox();
            this.levelName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // saveLevel
            // 
            this.saveLevel.Location = new System.Drawing.Point(15, 101);
            this.saveLevel.Name = "saveLevel";
            this.saveLevel.Size = new System.Drawing.Size(211, 32);
            this.saveLevel.TabIndex = 0;
            this.saveLevel.Text = "Create";
            this.saveLevel.UseVisualStyleBackColor = true;
            this.saveLevel.Click += new System.EventHandler(this.saveLevel_Click);
            // 
            // levelType
            // 
            this.levelType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.levelType.FormattingEnabled = true;
            this.levelType.Items.AddRange(new object[] {
            "Frontend (menu)",
            "Standard level"});
            this.levelType.Location = new System.Drawing.Point(15, 64);
            this.levelType.Name = "levelType";
            this.levelType.Size = new System.Drawing.Size(211, 21);
            this.levelType.TabIndex = 1;
            // 
            // levelName
            // 
            this.levelName.Location = new System.Drawing.Point(15, 25);
            this.levelName.Name = "levelName";
            this.levelName.Size = new System.Drawing.Size(211, 20);
            this.levelName.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(64, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Level Name";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 48);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Level Type";
            // 
            // CreateLevel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(240, 143);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.levelName);
            this.Controls.Add(this.levelType);
            this.Controls.Add(this.saveLevel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "CreateLevel";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Create Level";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button saveLevel;
        private System.Windows.Forms.ComboBox levelType;
        private System.Windows.Forms.TextBox levelName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
    }
}