package app;

import encryptionUtilities.*;

import java.awt.EventQueue;
import java.awt.Font;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;

public class AppUI {
	enum TypeOfOperation
	{
		TEXT,
		FILE,
		DIRECTORY
	};

	private JFrame frmEliteEncryption;
	private JCheckBox chckbxGhostMode;
	private JComboBox<String> typeToPreformOn;
	private JLabel lblOperationType;
	private JComboBox<String> encryptDecryptOption;
	private JLabel lblOperation;
	private JCheckBox chckbxOverwrite;
	
	private JButton btnContinue;
	private JButton btnDelKeys;
	private JButton btnCreateKeys;
	
	private JPanel numKeysPanel;
	private JComboBox<String> keyLenSelection;
	
	private JButton btnBack;
	
	private JPanel keyPanel;
	private JTextArea keyTextArea;
	private JScrollPane scrollPaneKeys;
	private JLabel lblKeys;
	
	private JPanel textPanel;
	private JTextArea textTextArea;
	private JScrollPane scrollPaneText;
	private JLabel lblText;
	
	private boolean overwriteFiles = false;
	private boolean ghostMode = false;
	private String uri = null;
	private ArrayList<String> filesList = new ArrayList<String>();
	private boolean encryptionOn = true;
	private int numKeys = 3;
	private int keyLen = 256;
	private TypeOfOperation typeOfOperation = TypeOfOperation.FILE;
	private int currentWindow = 0;
	private JLabel lblNumKeys;
	private JTextField textFieldNumKeys;
	private JLabel lblKeyLen;
	
	final private String[] bitLenOptions = new String[] {"256", "512", "1024", "2048"};
	
	// Encryption/Decryption classes
	private Utilities utils = new Utilities();
	private EEA eea = new EEA();

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					AppUI window = new AppUI();
					window.frmEliteEncryption.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public AppUI() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frmEliteEncryption = new JFrame();
		frmEliteEncryption.setTitle("Elite Encryption");
		//frmEliteEncryption.setBounds(100, 100, 550, 420);
		frmEliteEncryption.setBounds(100, 100, 450, 300);
		frmEliteEncryption.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frmEliteEncryption.getContentPane().setLayout(null);
		frmEliteEncryption.setResizable(false);
		
		/*
		 * First Page
		 */
		lblOperation = new JLabel("Operation:");
		lblOperation.setBounds(74, 28, 78, 16);
		frmEliteEncryption.getContentPane().add(lblOperation);
		
		encryptDecryptOption = new JComboBox<String>();
		encryptDecryptOption.addItemListener(new ItemListener() 
		{
			public void itemStateChanged(ItemEvent e) 
			{
				if(encryptDecryptOption.getSelectedItem() == "Encrypt" && chckbxGhostMode.isSelected())
					numKeysPanel.setVisible(true);
				else
					numKeysPanel.setVisible(false);
			}
		});
		encryptDecryptOption.setModel(new DefaultComboBoxModel<String>(new String[] {"Encrypt", "Decrypt"}));
		encryptDecryptOption.setSelectedIndex(0);
		encryptDecryptOption.setBounds(259, 24, 110, 27);
		frmEliteEncryption.getContentPane().add(encryptDecryptOption);
		
		lblOperationType = new JLabel("Type of operation:");
		lblOperationType.setBounds(74, 72, 128, 16);
		frmEliteEncryption.getContentPane().add(lblOperationType);
		
		typeToPreformOn = new JComboBox<String>();
		typeToPreformOn.addItemListener(new ItemListener() 
		{
			public void itemStateChanged(ItemEvent e) 
			{
				if(typeToPreformOn.getSelectedItem() == "Text")
					chckbxOverwrite.setVisible(false);
				else
					chckbxOverwrite.setVisible(true);
			}
		});
		typeToPreformOn.setModel(new DefaultComboBoxModel<String>(new String[] {"File", "Directory", "Text"}));
		typeToPreformOn.setSelectedIndex(0);
		typeToPreformOn.setBounds(259, 68, 110, 27);
		frmEliteEncryption.getContentPane().add(typeToPreformOn);
		
		btnContinue = new JButton("Continue");
		btnContinue.addActionListener(new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				switch(currentWindow)
				{
				case 0:
					continueToSecondWindow();
					//System.out.println(getValues());
					break;
				case 1:
					handleGhostModeText();
					break;
				case 2:
					handleGhostModeFilesAndDirectories();
					break;
				case 3:
					handleStandardText();
					break;
				case 4:
					handleFilesAndDirectories();
					break;
				default:
					break;
				}
			}
		});
		//btnContinue.setBounds(224, 357, 117, 29);
		btnContinue.setBounds(169, 237, 117, 29);
		frmEliteEncryption.getContentPane().add(btnContinue);
		
		chckbxOverwrite = new JCheckBox("Overwrite Files");
		chckbxOverwrite.setBounds(6, 100, 128, 23);
		chckbxOverwrite.setVisible(true);
		chckbxOverwrite.setToolTipText("The file(s) you encrypt/decrypt will be saved to the same file rather than creating a new one");
		frmEliteEncryption.getContentPane().add(chckbxOverwrite);
		
		chckbxGhostMode = new JCheckBox("Ghost Mode");
		chckbxGhostMode.addChangeListener(new ChangeListener() 
		{
			public void stateChanged(ChangeEvent e)
			{
				if(encryptDecryptOption.getSelectedItem() == "Encrypt" && chckbxGhostMode.isSelected())
					numKeysPanel.setVisible(true);
				else
					numKeysPanel.setVisible(false);
			}
		});
		chckbxGhostMode.setToolTipText("Ghost mode doesn't utilze a saved key file so the keys used only ever exist in memory "+
		"and can't ever be recovred via data recovery of your drive.");
		chckbxGhostMode.setBounds(6, 123, 128, 23);
		frmEliteEncryption.getContentPane().add(chckbxGhostMode);
		
		
		
		btnBack = new JButton("Back");
		btnBack.setBounds(6, 357, 78, 29);
		btnBack.setVisible(false);
		btnBack.addActionListener(new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				switch(currentWindow)
				{
				case 1:
					backToFirstWindow();
					break;
				default:
					backToFirstWindow();
					break;
				}
			}
		});
		frmEliteEncryption.getContentPane().add(btnBack);
		
		numKeysPanel = new JPanel();
		numKeysPanel.setBounds(169, 100, 200, 56);
		numKeysPanel.setVisible(false);
		numKeysPanel.setLayout(null);
		frmEliteEncryption.getContentPane().add(numKeysPanel);
		
		lblNumKeys = new JLabel("Number of Keys:");
		lblNumKeys.setToolTipText("The number of keys to generate");
		lblNumKeys.setBounds(0, 7, 117, 16);
		numKeysPanel.add(lblNumKeys);
		
		textFieldNumKeys = new JTextField();
		textFieldNumKeys.addFocusListener(new FocusAdapter() 
		{
			@Override
			public void focusLost(FocusEvent e) 
			{
				if(textFieldNumKeys.getText().equals(""))
					textFieldNumKeys.setText("3");
					
			}
		});
		textFieldNumKeys.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseClicked(MouseEvent e) 
			{
				textFieldNumKeys.setText("");
			}
		});
		textFieldNumKeys.setText("3");
		textFieldNumKeys.setToolTipText("The number of keys to generate");
		textFieldNumKeys.setBounds(160, 2, 32, 26);
		textFieldNumKeys.setColumns(10);
		numKeysPanel.add(textFieldNumKeys);
		
		lblKeyLen = new JLabel("Key Length:");
		lblKeyLen.setToolTipText("How many bits each key should be");
		lblKeyLen.setBounds(0, 35, 81, 16);
		numKeysPanel.add(lblKeyLen);
		
		keyLenSelection = new JComboBox<String>();
		keyLenSelection.setToolTipText("How many bits each key should be");
		keyLenSelection.setModel(new DefaultComboBoxModel<String>(bitLenOptions));
		keyLenSelection.setSelectedIndex(0);
		keyLenSelection.setBounds(100, 31, 94, 27);
		numKeysPanel.add(keyLenSelection);
		
		btnDelKeys = new JButton("Delete Keys");
		btnDelKeys.addActionListener(new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				if(utils.keysExist())
				{
					int x = JOptionPane.showConfirmDialog(frmEliteEncryption, "You are about to delete your keys file.\n"
							+ "By doing so you will no longer be able to decrypt messeages\npreviously encrypted with these keys\n\n"+
							"Are you sure you would like to continue?","Caution",JOptionPane.YES_OPTION);
					if(x == JOptionPane.YES_OPTION)
					{
						if(utils.deleteKeys())
							JOptionPane.showMessageDialog(frmEliteEncryption, "Your keys file was deleted successfully", 
									"Success",JOptionPane.INFORMATION_MESSAGE);
						else
							JOptionPane.showMessageDialog(frmEliteEncryption, "An error occured when trying to delete your keys file", 
									"ERROR",JOptionPane.ERROR_MESSAGE);
							
					}
				}
				else
					JOptionPane.showMessageDialog(frmEliteEncryption, "You do not currently have a keys file\nNo deletion occured", 
							"Information",JOptionPane.INFORMATION_MESSAGE);
					
			}
		});
		btnDelKeys.setBounds(74, 185, 117, 29);
		frmEliteEncryption.getContentPane().add(btnDelKeys);
		btnDelKeys.setToolTipText("Deletes current keys file");
		
		btnCreateKeys = new JButton("Create Keys");
		btnCreateKeys.addActionListener(new ActionListener() 
		{
			public void actionPerformed(ActionEvent e) 
			{
				boolean createNewKeys = true;
				if(utils.keysExist())
				{
					int x = JOptionPane.showConfirmDialog(frmEliteEncryption, "You already have a keys file.\n"
							+ "By overwriting it you will no longer be able to decrypt messeages\npreviously encrypted with these keys\n\n"+
							"Are you sure you would like to continue?","Caution",JOptionPane.YES_OPTION);
					if(x == JOptionPane.YES_OPTION)
					{
						utils.deleteKeys();
						createNewKeys = true;		
					}
					else
						createNewKeys = false;
				}
				if(createNewKeys)
				{
					boolean tryingToCreateKeys = true;
					do
					{
						try
						{
							String input = JOptionPane.showInputDialog(frmEliteEncryption, "Enter the desired number of keys: ", 
									"Create New Keys", JOptionPane.QUESTION_MESSAGE);
							if(input == null)
								tryingToCreateKeys = false;
							
							if(tryingToCreateKeys)
							{
								numKeys = Integer.parseInt(input);
								if(numKeys < 0)
								{
									numKeys = 0;
									JOptionPane.showMessageDialog(frmEliteEncryption, "The number of keys must be greater than or equal to 1", 
											"ERROR",JOptionPane.ERROR_MESSAGE);
								}
							}
						}
						catch(Exception e1)
						{
							JOptionPane.showMessageDialog(frmEliteEncryption, "Invalid Entry\nThe number of keys must be greater than or equal to 1", 
									"ERROR",JOptionPane.ERROR_MESSAGE);
							numKeys = 0;
							
						}
						
					}while(numKeys==0);
					
					if(tryingToCreateKeys)
					{
						Object input = JOptionPane.showInputDialog(frmEliteEncryption, "Enter a bit length for your keys", "Create New Keys",
								JOptionPane.QUESTION_MESSAGE, null, bitLenOptions, "OK");
						if(input == null)
							tryingToCreateKeys = false;
						else
							keyLen = Integer.parseInt(input.toString());
					}
					
					if(!tryingToCreateKeys)
					{
						numKeys = 3;
						keyLen = 256;
						JOptionPane.showMessageDialog(frmEliteEncryption, "Key Creation Aborted", 
								"Aborted",JOptionPane.INFORMATION_MESSAGE);
					}
					else
					{
						if(utils.writeKeys(eea.createKeys(numKeys, keyLen)))
							JOptionPane.showMessageDialog(frmEliteEncryption, "Your keys were generated and saved successfully", 
									"Success",JOptionPane.INFORMATION_MESSAGE);
						else
							JOptionPane.showMessageDialog(frmEliteEncryption, "An error occured when trying to save your keys", 
									"ERROR",JOptionPane.ERROR_MESSAGE);
					}
				}
					
			}
		});
		btnCreateKeys.setToolTipText("Creates a new keys file");
		btnCreateKeys.setBounds(252, 185, 117, 29);
		frmEliteEncryption.getContentPane().add(btnCreateKeys);
		
		/*
		 * Text Panel
		 */
		textPanel = new JPanel();
		textPanel.setBounds(6, 185, 538, 171);
		textPanel.setVisible(false);
		textPanel.setLayout(null);
		frmEliteEncryption.getContentPane().add(textPanel);
		
		textTextArea = new JTextArea();
		textTextArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
		textTextArea.setLineWrap(true);
		textTextArea.setWrapStyleWord(true);
		
		scrollPaneText = new JScrollPane();
		scrollPaneText.setBounds(10, 24, 515, 145);
		textPanel.add(scrollPaneText);
		scrollPaneText.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		scrollPaneText.setViewportView(textTextArea);
		
		lblText = new JLabel("Text:");
		lblText.setBounds(10, 6, 166, 16);
		textPanel.add(lblText);
		
		/*
		 * Keys Panel
		 */
		keyPanel = new JPanel();
		keyPanel.setBounds(6, 13, 538, 171);
		keyPanel.setVisible(false);
		keyPanel.setLayout(null);
		frmEliteEncryption.getContentPane().add(keyPanel);
		
		keyTextArea = new JTextArea();
		keyTextArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
		keyTextArea.setLineWrap(true);
		keyTextArea.setEditable(false);
		keyTextArea.setWrapStyleWord(true);
		
		scrollPaneKeys = new JScrollPane();
		scrollPaneKeys.setBounds(10, 24, 515, 145);
		keyPanel.add(scrollPaneKeys);
		scrollPaneKeys.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		scrollPaneKeys.setViewportView(keyTextArea);
		
		lblKeys = new JLabel("Keys:");
		lblKeys.setBounds(10, 6, 173, 16);
		keyPanel.add(lblKeys);
		
	}
	
//	private String getValues()
//	{
//		String out = "";
//		
//		if(encryptionOn)
//			out += "Encrypting\n";
//		else
//			out += "Decrypting\n";
//		
//	    switch(typeOfOperation)
//	    {
//	    case FILE:
//	    	out += "typeOfOperation: File\n";
//	    	break;
//	    case TEXT:
//	    	out += "typeOfOperation: Text\n";
//	    	break;
//	    default:
//	    	out += "typeOfOperation: Directory\n";
//	    	break;
//	    }
//	    out += "Overwrite Files: " + overwriteFiles + "\n";
//	    out += "Ghost Mode: " + ghostMode + "\n";
//	    out += "URI for File/Directory: " + uri + "\n";
//	    out += "Number of keys: " + numKeys + "\n";
//	    out += "Key length: " + keyLen + "\n";
//		
//		return out;
//	}
	
	private void getFilesInDirectory(File dirname)
	{
		File[] tempFilesList = dirname.listFiles();
		for(File f : tempFilesList)
		{
			if(f.isFile())
			{
				if(!encryptionOn)
				{
					if(f.toString().substring(f.toString().length()-4).equals(".eea"))
						filesList.add(f.toString());
				}
				else
					if(!f.toString().substring(f.toString().length()-4).equals(".eea"))
						filesList.add(f.toString());
			}
			else
				getFilesInDirectory(f);
		}
	}
	
	private void handleGhostModeText()
	{
		if(encryptionOn)
		{
			try
			{
				if(textTextArea.getText().equals(""))
					JOptionPane.showMessageDialog(frmEliteEncryption, "No text to encrypt was entered.\nPlease enter some text and try again", 
							"ERROR",JOptionPane.ERROR_MESSAGE);
				else
				{
					int x = JOptionPane.showConfirmDialog(frmEliteEncryption, "You are about to encrypt the text with one time use keys.\n"
							+ "You will be unable to decypt the text unless\nyou manually copy down the keys after you encrypt your message\n\n"+
							"Are you sure you would like to continue?","Caution",JOptionPane.YES_OPTION);
					if(x == JOptionPane.YES_OPTION)
					{
						String[] keys = eea.createKeys(numKeys, keyLen);
						String keysOutput = "";
						for(String k : keys)
							keysOutput += k+"\n";
						keyTextArea.setText(keysOutput);
						String encryptedText = new String(eea.encryptData(textTextArea.getText().getBytes(), keys));
						textTextArea.setText(encryptedText);
						lblText.setText("Encrypted Text:");
						btnContinue.setVisible(false);
					}
	
				}
			}
			catch(Exception e)
			{
				JOptionPane.showMessageDialog(frmEliteEncryption, "An error occured trying to encrypt your text.\n"+
					"If the problem persists, try saving your text to a .txt file and then try encrypting that", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
			}

		}
		else
		{
			try
			{
				if(textTextArea.getText().equals("") || keyTextArea.getText().equals(""))
					JOptionPane.showMessageDialog(frmEliteEncryption, "No text to decrypt and or keys to use were entered.\nPlease enter the encrypted text and or keys,"+
							" separated by newlines,\nand try again", "ERROR",JOptionPane.ERROR_MESSAGE);
				else
				{
					String[] keys = keyTextArea.getText().split("\n");
					if(eea.validKeyCheck(keys))
					{
						String keysOutput = "";
						for(String k : keys)
							keysOutput += k+"\n";
						keyTextArea.setText(keysOutput);
						String decryptedText = new String(eea.decryptData(textTextArea.getText().getBytes(), keys));
						textTextArea.setText(decryptedText);
						lblText.setText("Decrypted Text:");
						btnContinue.setVisible(false);
					}
					else
					{
						JOptionPane.showMessageDialog(frmEliteEncryption, "One or more keys you entered were invalid", 
								"ERROR",JOptionPane.ERROR_MESSAGE);
					}
	
				}
			}
			catch(Exception e)
			{
				JOptionPane.showMessageDialog(frmEliteEncryption, "An error occured trying to decrypt your text.\n"+
					"This is most likely due to a key mismatch, or there is too much text",
					"ERROR",JOptionPane.ERROR_MESSAGE);
			}
		}
	}
	
	private void handleStandardText()
	{
		if(utils.keysExist())
		{
			boolean success = false;
			if(encryptionOn)
			{
				try 
				{
					if(textTextArea.getText().equals(""))
						JOptionPane.showMessageDialog(frmEliteEncryption, "No text to encrypt was entered.\nPlease enter some text and try again", 
								"ERROR",JOptionPane.ERROR_MESSAGE);
					else
					{
						String encryptedText = new String(eea.encryptData(textTextArea.getText().getBytes(), utils.getKeys()));
						textTextArea.setText(encryptedText);
						lblText.setText("Encrypted Text:");
						success = true;
					}
				}
				catch(Exception e)
				{
					JOptionPane.showMessageDialog(frmEliteEncryption, "An error occured trying to encrypt your text.\n"+
						"If the problem persists, try saving your text to a .txt file and then try encrypting that", 
						"ERROR",JOptionPane.ERROR_MESSAGE);
					success = false;
				}
			}
			else
			{
				try 
				{
					if(textTextArea.getText().equals(""))
						JOptionPane.showMessageDialog(frmEliteEncryption, "No text to decrypt was entered.\nPlease enter the encrypted text and try again", 
								"ERROR",JOptionPane.ERROR_MESSAGE);
					else 
					{
						String decryptedText = new String(eea.decryptData(textTextArea.getText().getBytes(), utils.getKeys()));
						textTextArea.setText(decryptedText);
						lblText.setText("Decrypted Text:");
						success = true;
					}
				}
				catch(Exception e)
				{
					JOptionPane.showMessageDialog(frmEliteEncryption, "An error occured trying to decrypt your text.\n",
						"ERROR",JOptionPane.ERROR_MESSAGE);
					success = false;
				}
			}
			if(success)
				btnContinue.setVisible(false);
		}
		else
			JOptionPane.showMessageDialog(frmEliteEncryption, "You have no saved keys file\nYou can create one on the previous page", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
	}
	
	
	private String getFilesDirectoriesTextAreaText()
	{
		String infoText = "";
		if(encryptionOn)
		{
			if(ghostMode)
				infoText += "WARNING!\nYou are using ghost mode, which means the keys used to encrypt your files will not be saved."+
							"If you wish to be able to decrypt these files later you must manually copy down the keys once the encryption is completed.\n\n";
			if(typeOfOperation == TypeOfOperation.FILE)
				infoText += "You are about to encrypt the following file:\n"+uri+"\n\nPress continue to proceed...\n";
			else
			{
				filesList.clear();
				getFilesInDirectory(new File(uri));
				infoText += "You are about to encrypt the following files:\n";
				for (String f : filesList)
					infoText += f+"\n";
				infoText += "\n\nPress continue to proceed...\n";
			}
		}	
		else
		{
			if(typeOfOperation == TypeOfOperation.FILE)
				infoText += "You are about to decrypt the following file:\n"+uri+"\n\nPress continue to proceed...\n";
			else
			{
				filesList.clear();
				getFilesInDirectory(new File(uri));
				infoText += "You are about to decrypt the following files:\n";
				for (String f : filesList)
					infoText += f+"\n";
				infoText += "\n\nPress continue to proceed...\n";
			}
		}
		return infoText;
	}
	
	private void handleFilesAndDirectories()
	{
		boolean success = true;
		btnContinue.setEnabled(false);
		
		String[] keys;
		if(utils.keysExist())
		{
			keys = utils.getKeys();
			if(encryptionOn)
			{	
				if(typeOfOperation == TypeOfOperation.FILE)
					success = encryptFile(keys);
				else
					success = encryptDirectory(keys);
				
				if(success)
				{
					textTextArea.append("done.");
					if(typeOfOperation == TypeOfOperation.FILE)
						JOptionPane.showMessageDialog(frmEliteEncryption, "Your file was encrypted", 
								"Success",JOptionPane.INFORMATION_MESSAGE);
					else if(typeOfOperation == TypeOfOperation.DIRECTORY)
						JOptionPane.showMessageDialog(frmEliteEncryption, "All the files in your directory were encrypted", 
								"Success",JOptionPane.INFORMATION_MESSAGE);
				}
	
			}
			else
			{	
				if(typeOfOperation == TypeOfOperation.FILE)
					success = decryptFile(keys);
				else
					success = decryptDirectory(keys);
				
				if(success)
				{
					textTextArea.append("done.");
					if(typeOfOperation == TypeOfOperation.FILE)
						JOptionPane.showMessageDialog(frmEliteEncryption, "Your file was decrypted", 
								"Success",JOptionPane.INFORMATION_MESSAGE);
					else if(typeOfOperation == TypeOfOperation.DIRECTORY)
						JOptionPane.showMessageDialog(frmEliteEncryption, "All the files in your directory were decrypted", 
								"Success",JOptionPane.INFORMATION_MESSAGE);
				}
			}
			if(success)
				btnContinue.setVisible(false);
		}
		else
			JOptionPane.showMessageDialog(frmEliteEncryption, "You have no saved keys file\nYou can create one on the previous page", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
		btnContinue.setEnabled(true);
	}
	
	private void handleGhostModeFilesAndDirectories()
	{
		boolean success = true;
		btnContinue.setEnabled(false);
		if(encryptionOn)
		{
			String[] keys = eea.createKeys(numKeys, keyLen);
			String keysOutput = "";
			for(String k : keys)
				keysOutput += k+"\n";
			keyTextArea.setText(keysOutput);
							
			if(typeOfOperation == TypeOfOperation.FILE)
				success = encryptFile(keys);
			
			else
				success = encryptDirectory(keys);
			
			if(success)
			{
				textTextArea.append("done.");
				if(typeOfOperation == TypeOfOperation.FILE)
					JOptionPane.showMessageDialog(frmEliteEncryption, "Your file was encrypted", 
							"Success",JOptionPane.INFORMATION_MESSAGE);
				else if(typeOfOperation == TypeOfOperation.DIRECTORY)
					JOptionPane.showMessageDialog(frmEliteEncryption, "All the files in your directory were encrypted", 
							"Success",JOptionPane.INFORMATION_MESSAGE);
			}

		}
		else
		{
			if(keyTextArea.getText().equals(""))
				JOptionPane.showMessageDialog(frmEliteEncryption, "No keys to use were entered.\nPlease enter the keys,"+
						" separated by newlines,\nand try again", "ERROR",JOptionPane.ERROR_MESSAGE);
			else
			{
				String[] keys = keyTextArea.getText().split("\n");
				if(eea.validKeyCheck(keys))
				{
					String keysOutput = "";
					for(String k : keys)
						keysOutput += k+"\n";
					keyTextArea.setText(keysOutput);
					
					if(typeOfOperation == TypeOfOperation.FILE)
						success = decryptFile(keys);
					else
						success = decryptDirectory(keys);
					
					if(success)
					{
						textTextArea.append("done.");
						if(typeOfOperation == TypeOfOperation.FILE)
							JOptionPane.showMessageDialog(frmEliteEncryption, "Your file was decrypted", 
									"Success",JOptionPane.INFORMATION_MESSAGE);
						else if(typeOfOperation == TypeOfOperation.DIRECTORY)
							JOptionPane.showMessageDialog(frmEliteEncryption, "All the files in your directory were decrypted", 
									"Success",JOptionPane.INFORMATION_MESSAGE);
					}

				}
				else
				{
					JOptionPane.showMessageDialog(frmEliteEncryption, "One or more keys you entered were invalid", 
							"ERROR",JOptionPane.ERROR_MESSAGE);
					success = false;
				}

			}
		}
		btnContinue.setEnabled(true);
		btnContinue.setVisible(false);
	}
	
	private void continueToSecondWindow()
	{
		try 
		{
			overwriteFiles = chckbxOverwrite.isSelected();
			ghostMode = chckbxGhostMode.isSelected();
			encryptionOn = encryptDecryptOption.getSelectedItem().equals("Encrypt");
			
			if(typeToPreformOn.getSelectedItem().equals("File"))
				typeOfOperation = TypeOfOperation.FILE;
			else if(typeToPreformOn.getSelectedItem().equals("Directory"))
				typeOfOperation = TypeOfOperation.DIRECTORY;
			else
				typeOfOperation = TypeOfOperation.TEXT;
			
			if(typeOfOperation != TypeOfOperation.TEXT)
			{
				uri = promptForFiles(typeOfOperation);
				
				if(uri==null)
					throw new Exception("No File Selected");
				
				if(uri.equals("-1"))
					throw new Exception("Invalid File Type");
			}

			numKeys = Integer.parseInt(textFieldNumKeys.getText());
			if(numKeys<=0)
				throw new Exception("Invalid Entry");
			keyLen = Integer.parseInt(keyLenSelection.getSelectedItem().toString());
			
			btnBack.setVisible(true);
			setFirstPageVisability(false);
			
			textTextArea.setEditable(true);
			
			if(ghostMode)
			{
				keyPanel.setVisible(true);
				if(typeOfOperation == TypeOfOperation.TEXT)
				{
					textPanel.setVisible(true);
					keyPanel.setBounds(6, 13, 538, 171);
					textPanel.setBounds(6, 185, 538, 171);
					currentWindow = 1;
				}
				else
				{
					textPanel.setVisible(true);
					keyPanel.setBounds(6, 13, 538, 171);
					textPanel.setBounds(6, 185, 538, 171);
					currentWindow = 2;
				}
				
				if(encryptionOn)
				{
					keyTextArea.setEditable(false);
					lblKeys.setText("Keys used to encrypt: ");
					lblText.setText("Text to encrypt: ");
					if(typeOfOperation != TypeOfOperation.TEXT)
					{
						lblText.setText("");
						textTextArea.setText(getFilesDirectoriesTextAreaText());
					}
				}
				else
				{
					keyTextArea.setEditable(true);
					lblKeys.setText("Keys to use to decrypt: ");
					lblText.setText("Text to decrypt: ");
					if(typeOfOperation != TypeOfOperation.TEXT)
					{
						lblText.setText("");
						textTextArea.setText(getFilesDirectoriesTextAreaText());
					}
				}
			}
			else
			{
				keyPanel.setVisible(false);
				if(typeOfOperation == TypeOfOperation.TEXT)
				{
					textPanel.setVisible(true);
					textPanel.setBounds(6, 100, 538, 300);
					if(encryptionOn)
						lblText.setText("Text to encrypt: ");
					else
						lblText.setText("Text to decrypt: ");
					currentWindow = 3;
				}
				else
				{
					textPanel.setVisible(true);
					lblText.setText("");
					textPanel.setBounds(6, 100, 538, 300);
					textTextArea.setEditable(false);
					textTextArea.setText(getFilesDirectoriesTextAreaText());
					currentWindow = 4;
				}
			}
		}
		catch(Exception e)
		{
			if(e.getMessage().equals("Invalid File Type"))
			{
				JOptionPane.showMessageDialog(frmEliteEncryption, "Invalid File Type\nYou must select a file with the \'.eea\' file extention", 
						"ERROR",JOptionPane.ERROR_MESSAGE);
				backToFirstWindow();
			}
			else if(e.getMessage().equals("No File Selected"))
				backToFirstWindow();
			else
			{
				JOptionPane.showMessageDialog(frmEliteEncryption, "Invalid Entry\nThe number of keys must be greater than or equal to 1", 
						"ERROR",JOptionPane.ERROR_MESSAGE);
				numKeys = 3;
			}
		}
	}
	
	private void setFirstPageVisability(boolean b)
	{
		lblOperation.setVisible(b);
		lblOperationType.setVisible(b);
		encryptDecryptOption.setVisible(b);
		typeToPreformOn.setVisible(b);
		chckbxGhostMode.setVisible(b);
		btnCreateKeys.setVisible(b);
		btnDelKeys.setVisible(b);
		
		if(b)
		{
			if(typeOfOperation != TypeOfOperation.TEXT)
				chckbxOverwrite.setVisible(true);
			else
				chckbxOverwrite.setVisible(false);
			
			if(encryptionOn && ghostMode)
				numKeysPanel.setVisible(true);
			else
				numKeysPanel.setVisible(false);
			
			frmEliteEncryption.setSize(450, 300);
			btnContinue.setBounds(169, 237, 117, 29);
		}
		else
		{
			frmEliteEncryption.setSize(550, 420);
			btnContinue.setBounds(224, 357, 117, 29);
			chckbxOverwrite.setVisible(b);
			numKeysPanel.setVisible(b);
		}
	}
	
	private void backToFirstWindow()
	{
		setFirstPageVisability(true);
		btnBack.setVisible(false);
		keyPanel.setVisible(false);
		textPanel.setVisible(false);
		btnContinue.setVisible(true);
		textTextArea.setText("");
		keyTextArea.setText("");
		numKeys = 3;
		keyLen = 256;
		textFieldNumKeys.setText("3");
		keyLenSelection.setSelectedIndex(0);
		currentWindow = 0;
	}
	
	public String promptForFiles(TypeOfOperation type)
	{
	    JFileChooser fc = new JFileChooser();
	    switch(type)
	    {
	    case FILE:
	    	fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
	    	break;
	    default:
	    	fc.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY );
	    	break;
	    }
	    

	    if( fc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION )
	    {
	    	if(encryptionOn)
	    		return fc.getSelectedFile().getAbsolutePath();
	    	else
	    	{
	    		String path = fc.getSelectedFile().getAbsolutePath();
	    		if(path.substring(path.length()-4).equals(".eea") || typeOfOperation == TypeOfOperation.DIRECTORY)
	    			return path;
	    		else
	    			return "-1";
	    	}
	    }

	    return null;
	}

	private boolean encryptFile(String[] keys)
	{
		boolean success = true;
		try 
		{
			byte[] fileContents = Files.readAllBytes(Paths.get(uri));
			textTextArea.append("encrypting...\n");
			if(overwriteFiles)
			{
				File f = new File(uri);
				Files.write(Paths.get(uri+".eea"), eea.encryptData(fileContents, keys));
				f.delete();
			}
			else
				Files.write(Paths.get(uri+".eea"), eea.encryptData(fileContents, keys));

		} 
		catch (IOException e) 
		{
			e.printStackTrace();
			JOptionPane.showMessageDialog(frmEliteEncryption, "Error reading in the file", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
			success = false;
		}
		
		return success;
	}
	
	private boolean decryptFile(String[] keys)
	{
		boolean success = true;
		try 
		{
			byte[] fileContents = Files.readAllBytes(Paths.get(uri));
			textTextArea.append("decrypting...\n");
			File f = new File(uri);
			if(uri.substring(uri.length()-4).equals(".eea"))
				uri = uri.substring(0, uri.length()-4);
			if(overwriteFiles)
			{
				Files.write(Paths.get(uri), eea.decryptData(fileContents, keys));
				f.delete();
			}
			else
				Files.write(Paths.get(uri), eea.decryptData(fileContents, keys));

		} 
		catch (IOException e) 
		{
			e.printStackTrace();
			JOptionPane.showMessageDialog(frmEliteEncryption, "Error reading in the file", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
			success = false;
		}
		return success;
	}
	
	private boolean encryptDirectory(String[] keys)
	{
		boolean success = true;
		
		try
		{
			for(String file : filesList)
			{
				byte[] fileContents = Files.readAllBytes(Paths.get(file));
				textTextArea.append("Encrypting: "+file+"\n");
				if(overwriteFiles)
				{
					File f = new File(file);
					Files.write(Paths.get(file+".eea"), eea.encryptData(fileContents, keys));
					f.delete();
				}
				else
					Files.write(Paths.get(file+".eea"), eea.encryptData(fileContents, keys));		
			}
		}
		catch(IOException e)
		{
			e.printStackTrace();
			JOptionPane.showMessageDialog(frmEliteEncryption, "Error reading in the file", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
			success = false;
		}
		
		return success;
	}
	
	private boolean decryptDirectory(String[] keys)
	{
		boolean success = true;
		try 
		{
			for(String file : filesList)
			{
				byte[] fileContents = Files.readAllBytes(Paths.get(file));
				textTextArea.append("Decrypting: "+file+"\n");
				File f = new File(file);
				if(file.substring(file.length()-4).equals(".eea"))
					file = file.substring(0, file.length()-4);
				if(overwriteFiles)
				{
					Files.write(Paths.get(file), eea.decryptData(fileContents, keys));
					f.delete();
				}
				else
					Files.write(Paths.get(file), eea.decryptData(fileContents, keys));	
			}

		} 
		catch (IOException e) 
		{
			e.printStackTrace();
			JOptionPane.showMessageDialog(frmEliteEncryption, "Error reading in the file", 
					"ERROR",JOptionPane.ERROR_MESSAGE);
			success = false;
		}
		return success;
	}
}
