package encryptionUtilities;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Class of the implementation of the Elite Encryption Algorithm (EEA)
 * including generating keys, encrypting, and decrypting.
 * @author DarkAssassin23 - Will Jones
 *
 */
public class EEA 
{
	final private byte PADDING = 0;
	
	/**
	 * Algorithm for encrypting the data.
	 * First the key is XORed byte-by-byte against the data until the
	 * full length of the key has been used. Then, that block that 
	 * was just encrypted is then XORed byte-by-byte with the next block. 
	 * This process continues until all blocks have been encrypted. 
	 * The process then repeats for each key in the array.
	 * @param plainText The data to be encrypted
	 * @param keys The keys that will be used to do the encrypting
	 * @return Encrypted result
	 */
	public byte[] encryptData(byte[] plainText, String[] keys)
	{
		int blockSize = keys[0].getBytes().length;
		byte[][] keysList = new byte[keys.length][blockSize];
		for(int x=0;x<keys.length;x++)
			keysList[x] = keys[x].getBytes();
		
		byte[] previousBlock = new byte[blockSize];
		byte[] cipherText = new byte[(plainText.length + (blockSize-(plainText.length%blockSize)))];
		
		int loop = 0;
		for(byte[] key : keysList)
		{
			byte[] keyBlock = key;
			for(int x=0;x<cipherText.length;x++)
			{
				if(x!=0 && x%blockSize == 0)
					keyBlock = previousBlock;
				byte byteToXOR;
				
				if(loop==0)
				{
					if(x<plainText.length)
						byteToXOR = plainText[x];
					else
						byteToXOR = PADDING;
				}
				else
					byteToXOR = cipherText[x];
				
				cipherText[x] = (byte)(keyBlock[x%blockSize] ^ byteToXOR);
				previousBlock[x%blockSize] = (byte)(keyBlock[x%blockSize] ^ byteToXOR);
			}
			loop++;
		}
		
		return cipherText;
	}
	
	/**
	 * Algorithm for decrypting the data.
	 * First the algorithm pulls the second to last block in the sequence 
	 * and XOR's it with the last block. It progresses through the array 
	 * of bytes pealing off the block before the block it is decrypting
	 * before eventually it gets to the last block (or the first blcok since it
	 * is going in reverse) and XORs that with the respective key.
	 * It then repeats this process for each key in the array
	 * @param cyperText The data to be decrypted
	 * @param keys The keys that will be used to do the decrypting
	 * @return Decrypted result
	 */
	public byte[] decryptData(byte[] cipherText, String[] keys)
	{
		int blockSize = keys[0].getBytes().length;
		byte[][] keysList = new byte[keys.length][blockSize];
		for(int x=0;x<keys.length;x++)
			keysList[x] = keys[x].getBytes();

		byte[] plainText = new byte[cipherText.length];
		
		for(int x=keys.length-1;x>=0;x--)
		{
			if(x!=keys.length-1)
				cipherText = plainText;
			
			byte[] keyBlock = new byte[blockSize];
			if(cipherText.length >= (blockSize*2))
			{
				// Setup the keyBlock as the second to last block
				int start = cipherText.length - (blockSize*2);
				System.arraycopy(cipherText, start, keyBlock, 0, blockSize);
				
				int count = 0;
				int keyIndex = blockSize - 1;
				for(int i = cipherText.length-1; i >= blockSize; i--)
				{
					// Once you have completed a block
					// reset the key to one block before the one
					// you are about to xor
					if(count==blockSize)
					{
						System.arraycopy(cipherText, (i-((blockSize*2)-1)), keyBlock, 0, blockSize);
						count = 0;
						keyIndex = blockSize - 1;
					}
					
					plainText[i] = (byte)(keyBlock[keyIndex] ^ cipherText[i]);
					keyIndex--;
					count++;
				}
				
			}
			keyBlock = keysList[x];
			for(int i = blockSize-1; i>=0; i--)
				plainText[i] = (byte)(keyBlock[i] ^ cipherText[i]);
		}
		
		return removePadding(plainText, blockSize);
	}
	
	private byte[] removePadding(byte[] plainText, int blockSize)
	{
		byte[] result;
		
		int size = plainText.length;
		while(plainText[size-1] == PADDING)
			size--;
		
		result = new byte[size];
		System.arraycopy(plainText, 0, result, 0, size);
		
		return result;
	}
	
	/**
	 * Sets which hash algorithm will be used (SHA-256 or SHA-512) based on keyLength
	 * @param keyLength Key length to determine if SHA-256 or SHA-512 is used
	 * @return Hash algorithm to be used
	 */
	private MessageDigest getHashAlgorithm(int keyLength)
	{
		MessageDigest md = null;
		try 
		{
			switch(keyLength)
			{
			case 256:
				md = MessageDigest.getInstance("SHA-256");
				break;
			default:
				md = MessageDigest.getInstance("SHA-512");
				break;
			}
		}
		catch(NoSuchAlgorithmException e1)
		{
			e1.printStackTrace();
		}
		
		return md;
	}
	
	/**
	 * Algorithm to create new keys.
	 * Generates a random number and takes the SHA-256 or SHA-512 hash
	 * based on how long the user wanted their key to be and uses that as the key
	 * and generates as many keys as specified. In cases of 1024 and 2048-bit keys
	 * two or four, respectfully, SHA-512 hash keys are generated and merged into one
	 * @param numKeys Number of keys to generate
	 * @param keyLength Length of keys to generate via SHA-256 or SHA-512
	 * @return Array of keys
	 */
	public String[] createKeys(int numKeys, int keyLength)
	{
		String[] keys = new String[numKeys];
		MessageDigest md = getHashAlgorithm(keyLength);
			
		for(int x=0;x<numKeys;x++)
		{
			String totalKey = "";
			int length = 0;
			while(length<keyLength)
			{
				String seed = String.valueOf(Math.random()*Integer.MAX_VALUE);
				byte[] rawVal = md.digest(seed.getBytes());
				BigInteger key = new BigInteger(1, rawVal);
				StringBuilder temp = new StringBuilder(key.toString(16));
				
				while(temp.length() % 64 != 0)
					temp.insert(0, '0');

				totalKey += temp;
				length += 512;
			}
			
			keys[x] = totalKey;
		}

		return keys;
	}
	
	/**
	 * Takes in an array of keys, and returns whether or not 
	 * they are valid
	 * @param keys Array of keys to validate
	 * @return True if all keys are valid, False otherwise
	 */
	public boolean validKeyCheck(String[] keys)
	{
		for(String k : keys)
		{
			if(k.length()%128 != 0 && k.length() != 64)
				return false;
			try
			{
				new BigInteger(k, 16);
			}
			catch(Exception e)
			{
				return false;
			}
		}
		return true;
	}
}
