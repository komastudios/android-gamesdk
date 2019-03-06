// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/field_mask.proto
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
namespace Google.Protobuf.WellKnownTypes {

  /// <summary>Holder for reflection information generated from google/protobuf/field_mask.proto</summary>
  public static partial class FieldMaskReflection {

    #region Descriptor
    /// <summary>File descriptor for google/protobuf/field_mask.proto</summary>
    public static pbr::FileDescriptor Descriptor {
      get { return descriptor; }
    }
    private static pbr::FileDescriptor descriptor;

    static FieldMaskReflection() {
      byte[] descriptorData = global::System.Convert.FromBase64String(
          string.Concat(
            "CiBnb29nbGUvcHJvdG9idWYvZmllbGRfbWFzay5wcm90bxIPZ29vZ2xlLnBy",
            "b3RvYnVmIhoKCUZpZWxkTWFzaxINCgVwYXRocxgBIAMoCUJRChNjb20uZ29v",
            "Z2xlLnByb3RvYnVmQg5GaWVsZE1hc2tQcm90b1ABoAEBogIDR1BCqgIeR29v",
            "Z2xlLlByb3RvYnVmLldlbGxLbm93blR5cGVzYgZwcm90bzM="));
      descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
          new pbr::FileDescriptor[] { },
          new pbr::GeneratedClrTypeInfo(null, new pbr::GeneratedClrTypeInfo[] {
            new pbr::GeneratedClrTypeInfo(typeof(global::Google.Protobuf.WellKnownTypes.FieldMask), global::Google.Protobuf.WellKnownTypes.FieldMask.Parser, new[]{ "Paths" }, null, null, null)
          }));
    }
    #endregion

  }
  #region Messages
  /// <summary>
  ///  `FieldMask` represents a set of symbolic field paths, for example:
  ///
  ///      paths: "f.a"
  ///      paths: "f.b.d"
  ///
  ///  Here `f` represents a field in some root message, `a` and `b`
  ///  fields in the message found in `f`, and `d` a field found in the
  ///  message in `f.b`.
  ///
  ///  Field masks are used to specify a subset of fields that should be
  ///  returned by a get operation or modified by an update operation.
  ///  Field masks also have a custom JSON encoding (see below).
  ///
  ///  # Field Masks in Projections
  ///
  ///  When used in the context of a projection, a response message or
  ///  sub-message is filtered by the API to only contain those fields as
  ///  specified in the mask. For example, if the mask in the previous
  ///  example is applied to a response message as follows:
  ///
  ///      f {
  ///        a : 22
  ///        b {
  ///          d : 1
  ///          x : 2
  ///        }
  ///        y : 13
  ///      }
  ///      z: 8
  ///
  ///  The result will not contain specific values for fields x,y and z
  ///  (their value will be set to the default, and omitted in proto text
  ///  output):
  ///
  ///      f {
  ///        a : 22
  ///        b {
  ///          d : 1
  ///        }
  ///      }
  ///
  ///  A repeated field is not allowed except at the last position of a
  ///  field mask.
  ///
  ///  If a FieldMask object is not present in a get operation, the
  ///  operation applies to all fields (as if a FieldMask of all fields
  ///  had been specified).
  ///
  ///  Note that a field mask does not necessarily apply to the
  ///  top-level response message. In case of a REST get operation, the
  ///  field mask applies directly to the response, but in case of a REST
  ///  list operation, the mask instead applies to each individual message
  ///  in the returned resource list. In case of a REST custom method,
  ///  other definitions may be used. Where the mask applies will be
  ///  clearly documented together with its declaration in the API.  In
  ///  any case, the effect on the returned resource/resources is required
  ///  behavior for APIs.
  ///
  ///  # Field Masks in Update Operations
  ///
  ///  A field mask in update operations specifies which fields of the
  ///  targeted resource are going to be updated. The API is required
  ///  to only change the values of the fields as specified in the mask
  ///  and leave the others untouched. If a resource is passed in to
  ///  describe the updated values, the API ignores the values of all
  ///  fields not covered by the mask.
  ///
  ///  If a repeated field is specified for an update operation, the existing
  ///  repeated values in the target resource will be overwritten by the new values.
  ///  Note that a repeated field is only allowed in the last position of a field
  ///  mask.
  ///
  ///  If a sub-message is specified in the last position of the field mask for an
  ///  update operation, then the existing sub-message in the target resource is
  ///  overwritten. Given the target message:
  ///
  ///      f {
  ///        b {
  ///          d : 1
  ///          x : 2
  ///        }
  ///        c : 1
  ///      }
  ///
  ///  And an update message:
  ///
  ///      f {
  ///        b {
  ///          d : 10
  ///        }
  ///      }
  ///
  ///  then if the field mask is:
  ///
  ///   paths: "f.b"
  ///
  ///  then the result will be:
  ///
  ///      f {
  ///        b {
  ///          d : 10
  ///        }
  ///        c : 1
  ///      }
  ///
  ///  However, if the update mask was:
  ///
  ///   paths: "f.b.d"
  ///
  ///  then the result would be:
  ///
  ///      f {
  ///        b {
  ///          d : 10
  ///          x : 2
  ///        }
  ///        c : 1
  ///      }
  ///
  ///  In order to reset a field's value to the default, the field must
  ///  be in the mask and set to the default value in the provided resource.
  ///  Hence, in order to reset all fields of a resource, provide a default
  ///  instance of the resource and set all fields in the mask, or do
  ///  not provide a mask as described below.
  ///
  ///  If a field mask is not present on update, the operation applies to
  ///  all fields (as if a field mask of all fields has been specified).
  ///  Note that in the presence of schema evolution, this may mean that
  ///  fields the client does not know and has therefore not filled into
  ///  the request will be reset to their default. If this is unwanted
  ///  behavior, a specific service may require a client to always specify
  ///  a field mask, producing an error if not.
  ///
  ///  As with get operations, the location of the resource which
  ///  describes the updated values in the request message depends on the
  ///  operation kind. In any case, the effect of the field mask is
  ///  required to be honored by the API.
  ///
  ///  ## Considerations for HTTP REST
  ///
  ///  The HTTP kind of an update operation which uses a field mask must
  ///  be set to PATCH instead of PUT in order to satisfy HTTP semantics
  ///  (PUT must only be used for full updates).
  ///
  ///  # JSON Encoding of Field Masks
  ///
  ///  In JSON, a field mask is encoded as a single string where paths are
  ///  separated by a comma. Fields name in each path are converted
  ///  to/from lower-camel naming conventions.
  ///
  ///  As an example, consider the following message declarations:
  ///
  ///      message Profile {
  ///        User user = 1;
  ///        Photo photo = 2;
  ///      }
  ///      message User {
  ///        string display_name = 1;
  ///        string address = 2;
  ///      }
  ///
  ///  In proto a field mask for `Profile` may look as such:
  ///
  ///      mask {
  ///        paths: "user.display_name"
  ///        paths: "photo"
  ///      }
  ///
  ///  In JSON, the same mask is represented as below:
  ///
  ///      {
  ///        mask: "user.displayName,photo"
  ///      }
  ///
  ///  # Field Masks and Oneof Fields
  ///
  ///  Field masks treat fields in oneofs just as regular fields. Consider the
  ///  following message:
  ///
  ///      message SampleMessage {
  ///        oneof test_oneof {
  ///          string name = 4;
  ///          SubMessage sub_message = 9;
  ///        }
  ///      }
  ///
  ///  The field mask can be:
  ///
  ///      mask {
  ///        paths: "name"
  ///      }
  ///
  ///  Or:
  ///
  ///      mask {
  ///        paths: "sub_message"
  ///      }
  ///
  ///  Note that oneof type names ("test_oneof" in this case) cannot be used in
  ///  paths.
  /// </summary>
  public sealed partial class FieldMask : pb::IMessage<FieldMask> {
    private static readonly pb::MessageParser<FieldMask> _parser = new pb::MessageParser<FieldMask>(() => new FieldMask());
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pb::MessageParser<FieldMask> Parser { get { return _parser; } }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pbr::MessageDescriptor Descriptor {
      get { return global::Google.Protobuf.WellKnownTypes.FieldMaskReflection.Descriptor.MessageTypes[0]; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    pbr::MessageDescriptor pb::IMessage.Descriptor {
      get { return Descriptor; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public FieldMask() {
      OnConstruction();
    }

    partial void OnConstruction();

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public FieldMask(FieldMask other) : this() {
      paths_ = other.paths_.Clone();
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public FieldMask Clone() {
      return new FieldMask(this);
    }

    /// <summary>Field number for the "paths" field.</summary>
    public const int PathsFieldNumber = 1;
    private static readonly pb::FieldCodec<string> _repeated_paths_codec
        = pb::FieldCodec.ForString(10);
    private readonly pbc::RepeatedField<string> paths_ = new pbc::RepeatedField<string>();
    /// <summary>
    ///  The set of field mask paths.
    /// </summary>
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public pbc::RepeatedField<string> Paths {
      get { return paths_; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override bool Equals(object other) {
      return Equals(other as FieldMask);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public bool Equals(FieldMask other) {
      if (ReferenceEquals(other, null)) {
        return false;
      }
      if (ReferenceEquals(other, this)) {
        return true;
      }
      if(!paths_.Equals(other.paths_)) return false;
      return true;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override int GetHashCode() {
      int hash = 1;
      hash ^= paths_.GetHashCode();
      return hash;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override string ToString() {
      return pb::JsonFormatter.ToDiagnosticString(this);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void WriteTo(pb::CodedOutputStream output) {
      paths_.WriteTo(output, _repeated_paths_codec);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int CalculateSize() {
      int size = 0;
      size += paths_.CalculateSize(_repeated_paths_codec);
      return size;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(FieldMask other) {
      if (other == null) {
        return;
      }
      paths_.Add(other.paths_);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(pb::CodedInputStream input) {
      uint tag;
      while ((tag = input.ReadTag()) != 0) {
        switch(tag) {
          default:
            input.SkipLastField();
            break;
          case 10: {
            paths_.AddEntriesFrom(input, _repeated_paths_codec);
            break;
          }
        }
      }
    }

  }

  #endregion

}

#endregion Designer generated code
