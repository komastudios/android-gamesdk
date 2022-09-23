#!/bin/bash

# Updates objectivec_nsobject_methods.h by generating a list of all of the properties
# and methods on NSObject that Protobufs should not overload from iOS and macOS combined.
#
# The rules:
#   - No property should ever be overloaded.
#   - Do not overload any methods that have 0 args such as "autorelease".
#   - Do not overload any methods that start with "set[A-Z]" and have 1 arg such as
#     "setValuesForKeysWithDictionary:". Note that these will end up in the list as just
#     the "proto field" name, so "setValuesForKeysWithDictionary:" will become
#     "valuesForKeysWithDictionary".

set -eu

trim() {
    local var="$*"
    # remove leading whitespace characters
    var="${var#"${var%%[![:space:]]*}"}"
    # remove trailing whitespace characters
    var="${var%"${var##*[![:space:]]}"}"
    echo -n "$var"
}

objc_code=$(cat <<'END_CODE'
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

int main(int argc, const char * argv[]) {
  @autoreleasepool {
    Class cls = [NSObject class];

    // List out the protocols on NSObject just so we are aware if they change.
    unsigned int protocolCount;
    __unsafe_unretained Protocol **protocols =
        class_copyProtocolList(cls, &protocolCount);
    for (unsigned int i = 0; i < protocolCount; i++) {
      printf("// Protocol: %s\n", protocol_getName(protocols[i]));
    }
    free(protocols);

    // Grab all the properties.
    unsigned int propCount;
    objc_property_t *props = class_copyPropertyList(cls, &propCount);
    NSMutableSet *reservedNames = [[NSMutableSet alloc] init];
    for (unsigned int i = 0; i < propCount; ++i) {
      NSString *propertyName = [NSString stringWithUTF8String:property_getName(props[i])];
      [reservedNames addObject:propertyName];
    }
    free(props);

    // Note that methods have 2 defaults args (_cmd and SEL) so a method "0 arg method"
    // actually has 2.
    unsigned int methodCount;
    Method *methods = class_copyMethodList(cls, &methodCount);
    for (unsigned int i = 0; i < methodCount; ++i) {
      int argCount = method_getNumberOfArguments(methods[i]);
      NSString *methodName =
          [NSString stringWithUTF8String:sel_getName(method_getName(methods[i]))];
      if (argCount == 2) {
        [reservedNames addObject:methodName];
      }
      if (argCount == 3 && [methodName hasPrefix:@"set"] && methodName.length > 4) {
        NSString *firstLetter = [methodName substringWithRange:NSMakeRange(3,1)];
        NSString *lowerFirstLetter = [firstLetter lowercaseString];
        if ([lowerFirstLetter isEqual:firstLetter]) {
          // Make sure the next letter is a capital letter so we do not take things like
          // settingSomething:
          continue;
        }
        // -5 because 3 for set, 1 for the firstLetter and 1 for the colon on the end.
        NSString *restOfString =
            [methodName substringWithRange:NSMakeRange(4, methodName.length - 5)];
        methodName = [lowerFirstLetter stringByAppendingString:restOfString];
        [reservedNames addObject:methodName];
      }
    }
    free(methods);

    SEL sortSelector = @selector(caseInsensitiveCompare:);
    NSArray *array = [reservedNames.allObjects sortedArrayUsingSelector:sortSelector];
    for (NSString *item in array) {
      // Some items with _ in them get returned in quotes, so do not add more.
      if ([item hasPrefix:@"\""]) {
        printf("\t%s,\n", item.UTF8String);
      } else {
        printf("\t\"%s\",\n", item.UTF8String);
      }
    }
  }
  return 0;
}
END_CODE
)

file_header=$(cat <<'END_HEADER'
// NSObject methods
// Autogenerated by method_dump.sh. Do not edit by hand.
// Date: %DATE%
// macOS: %MACOS%
// iOS: %IOS%

const char* const kNSObjectMethodsList[] = {
END_HEADER
)

file_footer=$(cat <<'END_FOOTER'
};
END_FOOTER
)

# Check to make sure we are updating the correct file.
if [[ ! -e "objectivec_nsobject_methods.h" ]]; then
  echo "error: Must be run in the src/google/protobuf/compiler/objectivec directory"
  exit 1
fi

temp_dir=$(mktemp -d)

echo "$objc_code" >> "$temp_dir"/method_dump.m

# Compile up iphonesimulator and macos version of cmd line app.
iphone_simulator_sdk=$(xcrun --sdk iphonesimulator --show-sdk-path)
clang -isysroot "$iphone_simulator_sdk" -o "$temp_dir"/method_dump_ios \
    -framework Foundation -framework UIKit "$temp_dir"/method_dump.m
macos_sdk=$(xcrun --sdk macosx --show-sdk-path)
clang -isysroot "$macos_sdk" -o "$temp_dir"/method_dump_macos -framework Foundation \
    -framework Cocoa "$temp_dir"/method_dump.m

# Create a device of the latest phone and iphonesimulator SDK and run our iOS cmd line.
device_type=$(xcrun simctl list devicetypes | grep \.iPhone- | tail -1 | sed 's/.*(\(.*\))/\1/')
# runtimes come with a space at the end (for Xcode 10) so let's trim all of our input to
# be safe.
device_type=$(trim "$device_type")
runtime=$(xcrun simctl list runtimes | grep \.iOS- | tail -1 | \
    sed 's/.*\(com\.apple.\CoreSimulator\.SimRuntime\.iOS.*\)/\1/')
runtime=$(trim "$runtime")
uuid=$(uuidgen)
device_name="method_dump_device_$uuid"
device=$(xcrun simctl create "$device_name" "$device_type" "$runtime")
xcrun simctl spawn "$device" "$temp_dir"/method_dump_ios > "$temp_dir"/methods_unsorted_ios.txt
xcrun simctl delete "$device"

# Run the Mac version
"$temp_dir"/method_dump_macos >> "$temp_dir"/methods_unsorted_macos.txt

# Generate sorted output
echo "$file_header" | sed -e "s|%DATE%|$(date)|" -e "s|%MACOS%|$(basename $macos_sdk)|" \
    -e "s|%IOS%|$(basename $iphone_simulator_sdk)|" > "$temp_dir"/methods_sorted.txt
sort -u "$temp_dir"/methods_unsorted_ios.txt \
    "$temp_dir"/methods_unsorted_macos.txt >> "$temp_dir"/methods_sorted.txt
echo $"$file_footer" >> "$temp_dir"/methods_sorted.txt

# Check for differences. Turn off error checking because we expect diff to fail when
# there are no differences.
set +e
diff_out=$(diff -I "^//.*$" "$temp_dir"/methods_sorted.txt objectivec_nsobject_methods.h)
removed_methods=$(echo "$diff_out" | grep '^>.*$')
set -e
if [[ -n "$removed_methods" ]]; then
  echo "error: Methods removed from NSObject"
  echo "It appears that some methods may have been removed from NSObject."
  echo "This could mean that there may be some backwards compatibility issues."
  echo "You could potentially build apps that may not work on earlier systems than:"
  echo "$iphone_simulator_sdk"
  echo "$macos_sdk"
  echo "If they declare protobuf types that use any of the following as names:"
  echo "$removed_methods"
  echo ""
  echo "New Version: $temp_dir/methods_sorted.txt"
  echo "Old Version: objectivec_nsobject_methods.h"
  exit 1
fi
if [[ -n "$diff_out" ]]; then
  echo "Added Methods:"
  echo "$(echo "$diff_out" | grep '^<.*$' | sed -e 's/^< "\(.*\)",$/  \1/')"
fi;
cp "$temp_dir"/methods_sorted.txt objectivec_nsobject_methods.h
rm -rf "$temp_dir"
