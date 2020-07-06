#define TEST

#include <assert.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>


/*
 * Constructor : set up database input from a regular C file.
 */

DBInput :: DBInput (const char * name)
{
	if ((_in = fopen (name, "r")) != 0)
		_ownin = 1;
	else
		perror ("Unable to open input file");
}


/*
 * Destructor : if we own the output file, close it.
 */

DBInput :: ~ DBInput ()
{
	if (_ownin)
		fclose (_in);
}


/*
 * Virtual Private Instance : get the next input character.
 */

int DBInput :: nextChar (void)
{
	return _in == NULL ? EOF : getc (_in);
}


/*
 * Virtual Private Instance : return the last valid read character to the
 * input stream, or do nothing if EOF had been reached.
 *
 * Note that in all cases EXCEPT when ch == EOF, ch will have the same value
 * as the previously read character.
 */

void DBInput :: putBack (int ch)
{
	if (ch != EOF)
		ungetc (ch, _in);
}


/*
 * Virtual Instance : scan ahead to find out whether any data characters
 * remain in the input stream.
 */

int DBInput :: hasData (void)
{
	int ch;

	while ((ch = nextChar ()) != EOF && isspace (ch))
		;
	putBack (ch);

	return ch != EOF;
}


/*
 * Instance Private : consume whitespace, return next non-whitespace char.
 */

int DBInput :: eat_ws (void)
{
	int ch;

more:
	while ((ch = nextChar ()) != EOF && (ch == ' ' || ch == '\t'))
		;

	if (ch == '\\') {
		ch = nextChar ();
		if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ')
			goto more;

		putBack (ch);
		ch = '\\';
	}

	return ch;
}


/*
 * Virtual Instance : read a long integer in the indicated radix.
 *
 * The "Right Thing" to do when a number exceeds it's defined maximum size
 * or value can be a little difficult to define. Here, we can store an error
 * code in the input stream state, and peg the input value to it's maximum
 * permissible level.
 *
 * Like strtol (), if the radix passed to us is zero, then we will happily
 * attempt to infer a radix from the input stream.
 */

#define	_SIGNED		1
#define	_UNSIGNED	0

unsigned long DBInput :: readLong (int scale, unsigned long maxval)
{
	int ch, sign = 0, radix;
	unsigned long temp = 0;

	if ((ch = eat_ws ()) == '+')
		;
	else if (ch == '-')
		sign = -1;
	else if (! isdigit (ch) && ch != '.') {
		putBack (ch);
		return 0;
	} else
		putBack (ch);

	if ((ch = nextChar ()) == EOF)
		return 0;

	if (ch == '0') {
		if ((ch = nextChar ()) == EOF)
			return 0;
		if (ch == 'x' || ch == 'X')
			radix = 16;
		else {
			radix = 8;
			putBack (ch);
		}
	} else {
		radix = 10;
		putBack (ch);
	}

	while ((ch = nextChar ()) != EOF) {

		if (ch >= '0' && ch < (radix > 10 ? '9' + 1 : '0' + radix))
			ch -= '0';
		else if (ch >= 'A' && ch < radix + 'A' - 10)
			ch -= 'A' - 10;
		else if (ch >= 'a' && ch < radix + 'a' - 10)
			ch -= 'a' - 10;
		else
			break;		/* not a valid digit for this radix. */

		temp = temp <= ULONG_MAX / 10 ? temp * radix + ch : ULONG_MAX;
	}

	if (ch == '.')
   		while ((ch = nextChar ()) != EOF && isascii (ch) && isdigit (ch))
	   		if (scale > 0) {
				assert (temp < (ULONG_MAX / 10));
				temp = temp * 10 + ch - '0';
				   scale --;
			}

	while (scale > 0) {
		assert (temp < (ULONG_MAX / 10));
		temp *= 10;
		scale --;
	}

	putBack (ch);

	assert (temp <= maxval);

	return sign ? - temp : temp;
}


/*
 * Virtual Instance : read a string from the input; storing characters up to
 * the indicated length.
 */

int DBInput :: readString (char * string, size_t maxlen)
{
	int ch, temp;

	if ((ch = eat_ws ()) == '"') {
		while ((ch = nextChar ()) != EOF && ch != '"') {
			if (maxlen == 0)
				continue;

			if (ch == '\\') {
				if ((ch = nextChar ()) >= '0' && ch <= '7') {
					temp = ch - '0';

					if ((ch = nextChar ()) != EOF && ch >= '0' && ch <= '7') {
						temp = temp * 8 + ch - '0';
						if ((ch = nextChar ()) != EOF && ch >= '0' && ch <= '7') {
							temp = temp * 8 + ch - '0';
							ch = EOF;
						}
					}
					putBack (ch);

					/** Warn if temp > 255 **/

					ch = (char) temp;
				} else
					switch (ch) {

					case 'n':
						ch = '\n';
						break;

					case 'r':
						ch = '\r';
						break;

					case 't':
						ch = '\t';
						break;

					case 'b':
						ch = '\b';
						break;

					case 'a':
						ch = '\a';
						break;

					case 'v':
						ch = '\v';
						break;

					case 'f':
						ch = '\f';
						break;

					case 'x':
						if ((ch = nextChar ()) != EOF && isxdigit (ch)) {
							if (ch > '9')
								temp = ch - (ch >= 'a' ? 'a' : 'A') - 10;
							else
								temp = ch - '0';

							if ((ch = nextChar ()) != EOF && isxdigit (ch)) {
								if (ch > '9')
									ch -= (ch >= 'a' ? 'a' : 'A') - 10;
								else
									ch -= '0';
								temp = temp * 16 + ch;
								if ((ch = nextChar ()) != EOF && isxdigit (ch)) {
									if (ch > '9')
										ch -= (ch >= 'a' ? 'a' : 'A') - 10;
									else
										ch -= '0';
									temp = temp * 16 + ch;
									ch = EOF;
								}
							}
						}
						putBack (ch);

						/** Warn if temp > 255 **/

						ch = (char) temp;
						break;

					case '\n':
						continue;
					}
			}
		   	* string ++ = ch;
            maxlen --;
		}
	}

	if (ch != EOF && ch != '"')
		putBack (ch);

	while (maxlen > 0)
		* string ++ = ' ';

	return 0;
}


/*
 * Instance : see if the next input character is a separator or newline.
 */

int DBInput :: peek_sep (void)
{
	int ch;

#ifdef	TEST
	if (_in == NULL)
		return 0;
#endif

	ch = eat_ws ();
	putBack (ch);

	return ch == EOF || ch == '\n' || ch == '\r' || ch == _DB_SEP;
}


/*
 * Instance : read a long integer;
 */

DBInput & DBInput :: operator >> (long & longint)
{
	longint = (long) readLong (0, LONG_MAX);
	return * this;
}


/*
 * Instance : read an unsigned long integer.
 */

DBInput & DBInput :: operator >> (unsigned long & longint)
{
	longint = readLong (0, ULONG_MAX);
	return * this;
}


/*
 * Instance : read in a character.
 */

DBInput & DBInput :: operator >> (char & ch)
{
	ch = (char) readLong (0, CHAR_MAX);
	return * this;
}


/*
 * Instance : read in an unsigned character.
 */

DBInput & DBInput :: operator >> (unsigned char & ch)
{
	ch = (unsigned char) readLong (0, UCHAR_MAX);
	return * this;
}


/*
 * Instance : read in a short integer.
 */

DBInput & DBInput :: operator >> (short & shortint)
{
	shortint = (short) readLong (0, SHRT_MAX);
	return * this;
}


/*
 * Instance : read in an unsigned short integer.
 */

DBInput & DBInput :: operator >> (unsigned short & shortint)
{
	shortint = (unsigned short) readLong (0, USHRT_MAX);
	return * this;
}


/*
 * Instance : read in an integer.
 */

DBInput & DBInput :: operator >> (int & integer)
{
	integer = (int) readLong (0, INT_MAX);
	return * this;
}


/*
 * Instance : read a quoted string.
 */

DBInput & DBInput :: operator >> (char * string)
{
	readString (string, strlen (string));	/* use old string length */
	return * this;
}


/*
 * Manipulator : check for (and consume) a separator-character.
 */

DBInput & sep (DBInput & in)
{
	int ch;

#ifdef	TEST
	if (in._in == NULL)
		return in;
#endif

	ch = in.eat_ws ();

	if (ch != _DB_SEP)
		fprintf (stderr, "Bogus separator %d\n", ch);

	return in;
}


/*
 * Manipulator : check for (and consume) an EOL;
 */

DBInput & atend (DBInput & in)
{
	int ch;

#ifdef	TEST
	if (in._in == NULL)
		return in;
#endif

	ch = in.eat_ws ();

	if (ch != '\n' && ch != '\r')
		fprintf (stderr, "Expected newline, got %d\n", ch);

	return in;
}


/*
 * Manipulator : read the numeric value and an optional fixed-point part.
 */

DBInput & inscale (DBInput & in, short & number, int scale)
{
	number = (short) in.readLong (scale, SHRT_MAX);
	return in;
}


/*
 * Manipulator : read the numeric value and an optional fixed-point part.
 */

DBInput & inscale (DBInput & in, unsigned short & number, int scale)
{
	number = (unsigned short) in.readLong (scale, USHRT_MAX);
	return in;
}


/*
 * Manipulator : read the numeric value and an optional fixed-point part.
 */

DBInput & inscale (DBInput & in, int & number, int scale)
{
	number = (int) in.readLong (scale, INT_MAX);
	return in;
}


/*
 * Manipulator : read the numeric value and an optional fixed-point part.
 */

DBInput & inscale (DBInput & in, long & number, int scale)
{
	number = (long) in.readLong (scale, LONG_MAX);
	return in;
}


/*
 * Manipulator : read the numeric value and an optional fixed-point part.
 */

DBInput & inscale (DBInput & in, unsigned long & number, int scale)
{
	number = in.readLong (scale, ULONG_MAX);
	return in;
}


/*
 * Constructor : set up database output to a regular C file.
 */

DBOutput :: DBOutput (const char * name)
: _col (0), _ownout (0)
{
	if ((_out = fopen (name, "w")) != 0)
		_ownout = 1;
	else
		perror ("Unable to create output file");
}


/*
 * Destructor : if we own the output file, close it.
 */

DBOutput :: ~ DBOutput ()
{
	if (_ownout)
		fclose (_out);
}


/*
 * Virtual Instance : convert a number to ASCII decimal working
 * forward from the last byte in the buffer, then output the resulting string.
 *
 * NOTE : a 64-bit number has 18+ digits of precision, and we reserve two
 * more bytes for an optional decimal point and a NULL terminator.
 *
 * When the day comes that we can depend on running on a POSIX system (ie,
 * when Tandem get their act together) this should be rewritten to use
 * locales.
 */

DBOutput & DBOutput :: todec (unsigned long val, short scale, short precision)
{
	char buffer [21], * end = & buffer [sizeof (buffer) - 1];

	* end = '\0';
	do  {
		* -- end = ((int) (val % 10)) + '0';
		val /= 10;
		if (scale > 0 && -- scale == 0)
			* -- end = '.';
		if (precision > 0)
			precision --;
	} while (val > 0 || scale > 0);

	while (precision > 0) {
		precision --;
		* -- end = ' ';
	}

	fputs (end, _out);
	_col += (short) (& buffer [sizeof (buffer) - 1] - end);

	return * this;
}


/*
 * Virtual Instance : convert a signed number to decimal. This routine is
 * almost identical to the above, but with an extra place in the output
 * buffer reserved for a sign character.
 *
 * When the day comes that we can depend on running on a POSIX system (ie,
 * when Tandem get their act together) this should be rewritten to use
 * locales.
 */

DBOutput & DBOutput :: todec (long val, short scale, short precision)
{
	char buffer [22], * end = & buffer [sizeof (buffer) - 1];
	short sign;

	if ((sign = val < 0) != 0)
		val = - val;

	* end = '\0';
	do  {
		* -- end = ((int) (val % 10)) + '0';
		val /= 10;
		if (scale > 0 && -- scale == 0)
			* -- end = '.';
		if (precision > 0)
			precision --;
	} while (val > 0 || scale > 0);

	*  -- end = sign ? '-' : ' ';

	while (precision > 0) {
		precision --;
		* -- end = ' ';
	}

	fputs (end, _out);
	_col += (short) (& buffer [sizeof (buffer) - 1] - end);

	return * this;
}


/*
 * Virtual Instance : output a scaled short integer.
 */

DBOutput & DBOutput :: todec (short val, short scale, short precision)
{
	return todec ((long) val, scale, precision);
}


/*
 * Virtual Instance : output a scaled unsigned short integer.
 */

DBOutput & DBOutput :: todec (unsigned short val, short scale, short precision)
{
	return todec ((unsigned long) val, scale, precision);
}


/*
 * Virtual Instance : output a scaled integer.
 */

DBOutput & DBOutput :: todec (int val, short scale, short precision)
{
	return todec ((long) val, scale, precision);
}


/*
 * Virtual Instance : output a scaled unsigned integer.
 */

DBOutput & DBOutput :: todec (unsigned int val, short scale, short precision)
{
	return todec ((unsigned long) val, scale, precision);
}


/*
 * Virtual Instance : Output a quoted character string, using the normal "C"
 * escaping rules.
 */

DBOutput & DBOutput :: operator << (const char * string)
{
	int ch;
	static char _hexdig [] = "0123456789ABCDEF";

	if (string == NULL)
		return * this;

	putc ('"', _out);
	_col ++;

	while ((ch = * string ++) != 0) {
		if (! isascii (ch) || ! isprint (ch) || ch == '\\' || ch == '"') {
			putc ('\\', _out);
			_col ++;

			switch (ch) {
			case '\n':
				ch = 'n';
				break;

			case '\r':
				ch = 'r';
				break;

			case '\t':
				ch = 't';
				break;

			case '\b':
				ch = 'b';
				break;

			case '\a':
				ch = 'a';
				break;

			case '\v':
				ch = 'v';
				break;

			case '\f':
				ch = 'f';
				break;

			case '"':
			case '\\':
				break;

			default:	/* other non-printing character */
				putc ('x', _out);
				putc ('0', _out);
				putc (_hexdig [(ch >> 4) & 0xF], _out);
				ch = _hexdig [ch & 0xF];
				_col += 3;
			}
		}

		putc (ch, _out);
		_col ++;
	}

	putc ('"', _out);
	_col ++;

	return * this;
}


/*
 * Virtual Instance : output a separator.
 */

DBOutput & DBOutput :: sep (void)
{
	putc (_DB_SEP, _out);
	if (_col > 70) {
		putc ('\\', _out);
		putc ('\n', _out);
		_col = 0;
	}
	return * this;
}


/*
 * Virtual Instance : output an end-of-record.
 */

DBOutput & DBOutput :: endl (void)
{
	putc ('\n', _out);
	_col = 0;
	return * this;
}


/*
 * Manipulator : output a separator character.
 */

DBOutput & sep (DBOutput & out)
{
	return out.sep ();
}


/*
 * Manipulator : output a new-line character.
 */

DBOutput & endl (DBOutput & out)
{
	return out.endl ();
}

