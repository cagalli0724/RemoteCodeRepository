///////////////////////////////////////////////////////////////////////
//  MainWindow.xaml.cs - provide the function implementation for GUI//
//  ver 1.0                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr3, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines MainWindow class which provides the function implementation for GUI


Build Process:
==============
Required files
- CodePubisher.h, CodePublish.cpp, HideFuncHandle.cpp, HideFuncHandle.h

Build commands
- devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 04 May 2017
- first release
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;




namespace WPF_GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    
    public partial class MainWindow : Window
    {
        Shim shim = new Shim();
        Thread tRcv = null;
        String openFileName = "";
        public MainWindow()
        {
            InitializeComponent();
        }

        //for invoke adding categoryName into listbox
        void AddCategory(String msg)
        {

            CategorylistBox1.Items.Insert(0, msg);
            CategoryListBox2.Items.Insert(0, msg);
        }
        //for add file into file listbox
        void AddFile(String msg)
        {

            fileListBox.Items.Insert(0, msg);
        }
        //for upload the files
        private void Submitbutton_Click(object sender, RoutedEventArgs e)
        {
            if(fileBox.Text == "")
            {
                System.Windows.MessageBox.Show("Please select the file you want to upload!", "Error");
                return;
            }
            String msg = "3\n" + fileBox.Text + "\n";
            if (ListBoxrdButton.IsChecked == true)
            {
                if (CategorylistBox1.SelectedIndex == -1)
                {
                    System.Windows.MessageBox.Show("Please select the file category you want to upload!", "Error");

                }
                else
                {
                    msg = msg + CategorylistBox1.SelectedValue;
                    shim.PostMessage(msg);
                    CategorylistBox1.Items.Clear();
                    CategoryListBox2.Items.Clear();
                    shim.PostMessage("2\n");

                }
            }else
            {
                msg = msg + CtgtextBox.Text;
                shim.PostMessage(msg);
                CategorylistBox1.Items.Clear();
                CategoryListBox2.Items.Clear();
                shim.PostMessage("2\n");
            }
       
           
        }
        //for closing the GUI and send the quit message to sever to let sever close the related thread.
        private void Closing_Button(object sender, System.ComponentModel.CancelEventArgs e)
        {
            
            shim.PostMessage("quit");
        }
        //for delete the file
        private void DeleteFile_Click(object sender, RoutedEventArgs e)
        {
            if(fileListBox.SelectedIndex == -1)
            {
                System.Windows.MessageBox.Show("Please selete the file you want to delete!", "Error");

            }
            string file = fileListBox.SelectedItem.ToString();
            shim.PostMessage("4\n"+file);
            string category = CategoryListBox2.SelectedItem.ToString();
            CategorylistBox1.Items.Clear();
            CategoryListBox2.Items.Clear();
            shim.PostMessage("2\n");

            string msg = "5\n";
            if (category == "<No parents>")
            {
                msg = msg + "\n";
            }
            else if (category == "<All files>")
            {
                msg = msg + "<...>\n";
            }
            else
            {
                msg = msg + category + "\n";
            }
            fileListBox.Items.Clear();
            shim.PostMessage(msg);
        }
        //for open the file to download the files.
        private void OpenFile_Click(object sender, RoutedEventArgs e)
        {
            if(fileListBox.SelectedIndex == -1)
            {
                System.Windows.MessageBox.Show("Please select the file to open!", "Error");
                return;
            }
            else
            {
                //shim.SetPublishDir(str);
                openFileName = fileListBox.SelectedItem.ToString();
                String msg = "8\n" + fileListBox.SelectedItem.ToString();
                shim.PostMessage(msg);
            }
        }

        //for browsing the computer to choose the file to upload.
        private void Browse_Click(object sender, RoutedEventArgs e)
        {
            // Create OpenFileDialog 
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

            // Set filter for file extension and default file extension 
            dlg.DefaultExt = ".cpp";
            dlg.Filter = "Cpp Files (*.cpp)|*.cpp|Header Files (*.h)|*.h";


            // Display OpenFileDialog by calling ShowDialog method 
            Nullable<bool> result = dlg.ShowDialog();


            // Get the selected file name and display in a TextBox 
            if (result == true)
            {
                // Open document 
                String filename = dlg.FileName;
                fileBox.Text = filename;
            }
        }
        //for window loading
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {   //create the thread for dealing with the message got from server.
            tRcv = new Thread(
              () =>
              {
                  while (true){
                      String msg = shim.GetMessage();
                      if (msg == "quit") {
                          break;
                      }
                      if(msg == "C") {
                          String str = shim.GetMessage();
                          while(str != ".") {
                              if(str == "") {
                                  str = "<No parents>";
                              }
                              Action<String> act = new Action<string>(AddCategory);
                              Object[] args = { str };
                              Dispatcher.Invoke(act, args);
                              str = shim.GetMessage();
                          }
                          Action<String> act2 = new Action<string>(AddCategory);
                          Object[] args2 = { "<All files>" };
                          Dispatcher.Invoke(act2, args2);
                      }
                      if (msg == "F")
                      {
                          String str = shim.GetMessage();
                          while (str != "."){
                              Action<String> act = new Action<string>(AddFile);
                              Object[] args = { str };
                              Dispatcher.Invoke(act, args);
                              str = shim.GetMessage();
                          }
                      }
                      if (msg == "O") {  //the 'lazy download' completion, we will open the webpage.
                          String str = shim.GetMessage();
                          str = str + "\\" + openFileName+".htm";
                          System.Diagnostics.Process.Start(str);
                      }
                  }
              }
            );
            shim.PostMessage("1\n");
            shim.PostMessage("2\n");
            tRcv.Start();
        }
        //get the file list acccording to the category.
        private void Category_Selected(object sender, SelectionChangedEventArgs e)
        {
            if((sender as System.Windows.Controls.ListBox).SelectedIndex == -1)
            {
                return;
            }
            String categorySelect = ((sender as System.Windows.Controls.ListBox).SelectedItem.ToString());
            String msg = "5\n";
            if(categorySelect == "<No parents>")
            {
                msg = msg + "\n";
            }else if(categorySelect == "<All files>")
            {
                msg = msg + "<...>\n";
            }
            else
            {
                msg = msg + categorySelect+"\n";
            }
            fileListBox.Items.Clear();
            shim.PostMessage(msg);
          
        }

        //get the category list from server.
        private void CategoryButton_Click(object sender, RoutedEventArgs e)
        {
            CategorylistBox1.Items.Clear();
            CategoryListBox2.Items.Clear();
            String msg = "2\n";
            shim.PostMessage(msg);
        }

    }
}
