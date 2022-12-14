# Broma

Broma is a cpp-like language designed for automatically generating cross-platform address bindings for classes and functions. Here is an example:

```cpp
	class BindedClass {
		void bindedFunction() = mac 0xd5db0, win 0x3c8d, ios 0xa83bc;

		int m_member1;
		int m_member2;
	}

	class OtherBindedClass : BindedClass {
		virtual void otherBindedFunction() = mac 0x7e3bc, win 0x5a1c, ios 0x8e412;

		// win and ios addresses have not been found yet, will not generate
		static int staticFunction(int a, bool c) = mac 0x74bd3;

		// Embed c++ code
		inline int getIndex(int index) {
			return m_myVector[index];
		} 

		// templates supported
		std::vector<int> m_myVector;
	}
```

This language is most notably used in the Geometry Dash modding framework [Geode](https://github.com/geode-sdk/geode), and most langauge features here are tailored to that.