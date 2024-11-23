# 02 - HARD CHALLENGE

## CHALLENGE 1 ( UNLOCKED )

### HINT
```
Please provide the name of the service the phishing campaign is targeting in such format `WPCTF{service_url}`
```

### SOLUTION
```
WPCTF{https://satayo.cloud/}
```
![001.png](images/001.png)  
To identify the given service the analyst needs to identify the logo or insert an email in order to retrieve the URL of Satayo service. The flag will be SATAYO the `WPCTF{https://satayo.cloud/}` of the service.


## CHALLENGE 2 ( LOCKED - NEED CHALLENGE 1 )

### HINT
```
What is the email of the developer (author) of the phishing kit?
```

### SOLUTION
```
WPCTF{evil.evil.evil.evil.evil.999@protonmail.com}
```
For retrieving the email of the developer, the analyst needs to understand that the phishing kit was misconfigured. It exposes a .git directory. USing tool or scripts for resolving it the challenger needs to download it.

![001a.png](images/001a.png)

Inspecting the project's git history makes it straightforward to identify the developer's email address.  

![002.png](images/002.png)

Using git command ```git log```

![003.png](images/003.png)

## CHALLENGE 3 ( LOCKED - NEED CHALLENGE 2 )

### HINT
```
What is the email of the threat actor?
```

### SOLUTION
```
WPCTF{belzebuohm@gmail.com}
```

Using git command ```git show a2ecde0e4917788fc258b3b2e6f674cb4f60ac71```

![004.png](images/004.png)

From the retrieved token and chat history, it becomes possible to access the complete conversations of the bot.  

![005.png](images/005.png)

Analysts can utilize tools such as [Telegram bot dumper](https://github.com/soxoj/telegram-bot-dumper) to extract messages intended for testing the kit. OSINT, or Open Source Intelligence, involves recognizing that the kit may be sold by a developer and subsequently installed by a Threat Actor. This understanding is a crucial step in addressing the flag effectively. 

![006.png](images/006.png)

The date of capture is, however, disclosed. The photo were taken in date 25-10-2024.

![009.png](images/009.png)

## CHALLENGE 4 ( LOCKED - NEED CHALLENGE 3 )

### HINT
```
What event did the developer attend(ed)? In detail, what/who was the main attraction?
```

### SOLUTION
```
WPCTF{John Maus}
```

tarting from the email evil.evil.evil.evil.evil.999@protonmail.com , the analyst must understand that the email address present a pretty unique key used for registering it.


Pivoting from the address evil.evil.evil.evil.evil.999, participants can identify the corresponding Gmail address as evil.evil.evil.evil.evil.999@gmail.com.

This email was utilized to register a Flickr account. The process of identification is straightforward. By employing tools like Holehe, participants can uncover social media accounts associated with the identified email.
GitHub - megadose/holehe: holehe allows you to check if the mail is used on different sites like twitter, instagram and will retrieve information on sites with the forgotten password function.

A manual method for identifying user accounts involves retrieving information from the Flickr API.
Email to Flickr account - OSINT tutorials by Aware Online

A faster approach will using a payed service like OSINT Industries | Powerful Tools for Accurate Investigations.

![007.png](images/007.png)

The Flickr profile exposes 2 photos. Both of them aren’t useful for image reverse search.

![008.png](images/008.png)

The date of capture is, however, disclosed. The photo were taken in date 25-10-2024.

![009.png](images/009.png)

Going further and analyzing the given photo, let the participants identify the venue of the concert that is Covo Club in Bologna.

![010.png](images/010.png)

From a reverse image search, the analyst can identify the location as Covo Club.    
![011.png](images/011.png)

![012.png](images/012.png)

Looking forward searching for the event and date of the shoot let the analyst to identify the name of John Maus the last osint flag of the the comptetion.

![013.png](images/013.png)

## CHALLENGE 5 ( LOCKED - NEED CHALLENGE 4 )

### HINT
```
What is the favorite game of the phishing kit developer?
```

### SOLUTION
```
WPCTF{DOOM}
```

In the source code of the frontend page of the kit there is a strange JavaScript lib, pretty obfuscated.

![014.png](images/014.png)

![018.png](images/018.png)

Using tools like [JavaScript Deobfuscator](https://deobfuscate.relative.im/) the analyst will be able to retrieve the original algorithm. 

![019.png](images/019.png)

The library features an exciting easter egg: when users input the keystrokes IDDQD, a well-known cheat code from DOOM, the face of the main character appears on the frontend. This face changes throughout the day, specifically between 9 AM and 6 PM. As the end of the challenge draws near, the character's expression will become increasingly pained, reflecting the intensity of the situation.  

![020.png](images/020.png)

Searching for the iconic face of the main character from DOOM, the analyst can identify the game as the developer's favorite.

