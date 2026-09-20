/*
JEspressoVM - project to bring java bytecode execution to esp32 (and others)

Copyright (C) 2026  Vladislav Potrashkov

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/

package java.lang;

public final class Integer{
    public static final int MIN_VALUE = 0x80000000;
    public static final int MAX_VALUE = 0x7FFFFFFF;
    public static final int SIZE = 32;

    private final int value;

    public Integer(int value){
        this.value = value;
    }

    public Integer(String s){
        this.value = parseInt(s);
    }

    public int intValue(){
        return value;
    }

    public long longValue(){
        return (long)value;
    }

    public float floatValue(){
        return (float)value;
    }

    public double doubleValue(){
        return (double)value;
    }

    public boolean equals(Object obj){
        if(obj instanceof Integer){
            return value == ((Integer)obj).value;
        }

        return false;
    }

    public int hashCode(){
        return value;
    }

    public String toString(){
        return toString(value);
    }

    public static Integer valueOf(int i){
        return new Integer(i);
    }

    public static int parseInt(String s){
        return parseInt(s, 10);
    }

    public static int parseInt(String s, int radix){
        if(s == null) throw new NumberFormatException();
        if(radix < 2 || radix > 36) throw new NumberFormatException();

        char chars[] = s.toCharArray();
        int len = chars.length;
        if(len == 0) throw new NumberFormatException();

        boolean negative = false;
        int i = 0;
        char first = chars[0];
        if(first == '-'){
            negative = true;
            i = 1;
        } else if(first == '+'){
            i = 1;
        }

        if(i >= len) throw new NumberFormatException();

        int result = 0;
        while(i < len){
            char c = chars[i];
            int digit;

            if(c >= '0' && c <= '9') digit = c - '0';
            else if(c >= 'a' && c <= 'z') digit = c - 'a' + 10;
            else if(c >= 'A' && c <= 'Z') digit = c - 'A' + 10;
            else throw new NumberFormatException();

            if(digit >= radix) throw new NumberFormatException();

            result = result * radix + digit;
            i++;
        }

        return negative ? -result : result;
    }

    public static String toString(int i){
        if(i == 0) return "0";

        boolean negative = false;
        if(i < 0){
            negative = true;
            if(i == MIN_VALUE) return "-2147483648";
            i = -i;
        }

        char buf[] = new char[11];
        int pos = 11;

        while(i > 0){
            buf[--pos] = (char)('0' + (i % 10));
            i /= 10;
        }

        if(negative){
            buf[--pos] = '-';
        }

        int len = 11 - pos;
        char result[] = new char[len];
        System.arraycopy(buf, pos, result, 0, len);

        return new String(result);
    }
}