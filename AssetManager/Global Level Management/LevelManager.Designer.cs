namespace AssetManager
{
    partial class LevelManager
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
            this.addLevel = new System.Windows.Forms.Button();
            this.manageLevel = new System.Windows.Forms.Button();
            this.deleteLevel = new System.Windows.Forms.Button();
            this.levelList = new System.Windows.Forms.ListBox();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // addLevel
            // 
            this.addLevel.Location = new System.Drawing.Point(491, 133);
            this.addLevel.Name = "addLevel";
            this.addLevel.Size = new System.Drawing.Size(164, 34);
            this.addLevel.TabIndex = 0;
            this.addLevel.Text = "Create New Level";
            this.addLevel.UseVisualStyleBackColor = true;
            this.addLevel.Click += new System.EventHandler(this.addLevel_Click);
            // 
            // manageLevel
            // 
            this.manageLevel.Location = new System.Drawing.Point(491, 173);
            this.manageLevel.Name = "manageLevel";
            this.manageLevel.Size = new System.Drawing.Size(164, 34);
            this.manageLevel.TabIndex = 1;
            this.manageLevel.Text = "Manage Selected Level";
            this.manageLevel.UseVisualStyleBackColor = true;
            this.manageLevel.Click += new System.EventHandler(this.manageLevel_Click);
            // 
            // deleteLevel
            // 
            this.deleteLevel.Location = new System.Drawing.Point(491, 213);
            this.deleteLevel.Name = "deleteLevel";
            this.deleteLevel.Size = new System.Drawing.Size(164, 34);
            this.deleteLevel.TabIndex = 2;
            this.deleteLevel.Text = "Delete Selected Level";
            this.deleteLevel.UseVisualStyleBackColor = true;
            this.deleteLevel.Click += new System.EventHandler(this.deleteLevel_Click);
            // 
            // levelList
            // 
            this.levelList.FormattingEnabled = true;
            this.levelList.Location = new System.Drawing.Point(7, 9);
            this.levelList.Name = "levelList";
            this.levelList.Size = new System.Drawing.Size(478, 238);
            this.levelList.TabIndex = 3;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(544, 33);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(74, 34);
            this.button1.TabIndex = 4;
            this.button1.Text = "TEST";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // LevelManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(661, 254);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.levelList);
            this.Controls.Add(this.deleteLevel);
            this.Controls.Add(this.manageLevel);
            this.Controls.Add(this.addLevel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "LevelManager";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Level Manager";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button addLevel;
        private System.Windows.Forms.Button manageLevel;
        private System.Windows.Forms.Button deleteLevel;
        private System.Windows.Forms.ListBox levelList;
        private System.Windows.Forms.Button button1;
    }
}

