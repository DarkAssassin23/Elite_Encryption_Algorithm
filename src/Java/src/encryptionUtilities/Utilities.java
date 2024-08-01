package encryptionUtilities;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.IllegalArgumentException;
import java.lang.NullPointerException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Base64;
import java.util.Scanner;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

/**
 * Class of utilities used for reading and writing the keys
 * checking if the key file exists, and deleting the key file
 * @author DarkAssassin23 - Will Jones
 *
 */
public class Utilities implements Serializable
{
	// Added to fix Serializable warning
	private static final long serialVersionUID = 1L;
	
	final private Base64.Encoder base64Encode = Base64.getEncoder();
	final private Base64.Decoder base64Decode = Base64.getDecoder();
	// Fixes bug where double-clicking on the .jar file would
	// set the working directory as a temproary location and 
	// not where the file is
	static final String legacyKeyFilePath = new File(Utilities.class.getProtectionDomain().getCodeSource().getLocation().getPath()).toString().replace("EEA_App.jar", "keys.ser");
	
	/**
	 * Saves the the array of keys as a serialized object into keys.ser
	 * @param keys Array of keys to save
	 * @return Returns true if the operation was successful
	 */
	public boolean writeKeys(String[] keys)
	{
		JFileChooser fc = new JFileChooser();
		FileFilter filter = new FileNameExtensionFilter("Keys File", "keys");
		fc.setFileFilter(filter);
		fc.setDialogTitle("Select where to save the keys file");
		fc.showSaveDialog(null);
		File keysFile = fc.getSelectedFile();
		try 
		{
			if (!keysFile.getName().endsWith(".keys"))
				keysFile = new File(keysFile.getAbsolutePath() + ".keys");
			if (keysFile.exists())
			{
				int x = JOptionPane.showConfirmDialog(null, "The keys file you entered already exists.\n"+
							"Are you sure you would like to continue?","Caution",JOptionPane.YES_OPTION);
				if (x != JOptionPane.YES_OPTION)
					return false;
			}

			FileWriter writer = new FileWriter(keysFile);
			String allKeys = new String();
			for (String k : keys)
			{
				if (!allKeys.isEmpty())
					allKeys += "\n";
				allKeys += k;
			}
			writer.write(base64Encode.encodeToString(allKeys.getBytes()));
			writer.close();
		} 
		catch (FileNotFoundException e1) 
		{
			e1.printStackTrace();
			return false;
		}
		catch (IOException e1) 
		{
			e1.printStackTrace();
			return false;
		}
		catch (NullPointerException e)
		{
			System.out.println("Keys file save aborted");
			return false;
		}
		return true;
	}
	
	/**
	 * Reads in a serialized array of keys from keys.ser
	 * @return Array of keys
	 */
	private String[] readKeys()
	{
		String[] keys = null;
		// Legacy 
		if (keysExist())
		{
			try 
			{
				ObjectInputStream objInputStream = new ObjectInputStream(new FileInputStream(legacyKeyFilePath));
				keys = (String[]) objInputStream.readObject();
				objInputStream.close();
			} 
			catch (IOException e1) 
			{
				e1.printStackTrace();
			} 
			catch (ClassNotFoundException e1) 
			{
				e1.printStackTrace();
			}
			return keys;
		}
		
		JFileChooser fc = new JFileChooser();
		FileFilter filter = new FileNameExtensionFilter("Keys File", "keys");
		fc.setFileFilter(filter);
		fc.setDialogTitle("Select the keys file to use");
		fc.showOpenDialog(null);
		File keysFile = fc.getSelectedFile();
		try
		{
			if (!keysFile.getName().endsWith(".keys"))
			{
				JOptionPane.showMessageDialog(null, "The file selected is not a keys file.", 
									          "ERROR",JOptionPane.ERROR_MESSAGE);
				return keys;
			}
			byte[] data = Files.readAllBytes(Paths.get(keysFile.getAbsolutePath()));
			byte[] decoded = base64Decode.decode(data);
			keys = new String(decoded).split("\n");
		}
		catch (IOException e1) 
		{
			e1.printStackTrace();
		}
		catch (IllegalArgumentException e)
		{
			e.printStackTrace();
		}
		catch (NullPointerException e)
		{
			System.out.println("Keys file selection aborted");
		}
		return keys;
	}
	
	/**
	 * Checks if the keys.ser file exits
	 * @return If the keys.ser file exits
	 */
	public boolean keysExist()
	{
		File file = new File(legacyKeyFilePath);
		return file.exists();
	}
	
	/**
	 * If the keys.ser file exists, it reads them in and returns them
	 * otherwise it returns null
	 * @return Array of keys, or null if no key file exits
	 */
	public String[] getKeys()
	{	
		// if(!keysExist())
		// {
		// 	String[] keys = null;
			
		// 	return keys;	
		// }
		
		return readKeys();
	}
	
	/**
	 * Deletes the keys.ser file containing the keys
	 * @return If the deletion was successful
	 */
	public boolean deleteKeys()
	{
		File file = new File(legacyKeyFilePath);
		return file.delete();
	}
}
