require 'test/unit'
require File.expand_path('../../ext/uuid4r.so', __FILE__)

class UUIDTest < Test::Unit::TestCase
  def test_uuid_v1_default
    assert_kind_of(String, UUID4R::uuid_v1)
    assert_kind_of(String, UUID4R::uuid(1))
  end
  
  def test_uuid_v4_default
    assert_kind_of(String, UUID4R::uuid_v4)
    assert_kind_of(String, UUID4R::uuid(4))
  end
  
  def test_uuid_v1_str
    assert_kind_of(String, UUID4R::uuid_v1(:str))
    assert(UUID4R::uuid_v1(:str).length > 0)
    assert_not_equal(UUID4R::uuid_v1(:str), UUID4R::uuid_v1(:str))
  end

  def test_uuid_v1_bin
    assert_kind_of(String, UUID4R::uuid_v1(:bin))
    assert_equal(16, UUID4R::uuid_v1(:bin).length)
  end

  def test_uuid_v1_txt
    assert_kind_of(String, UUID4R::uuid_v1(:txt))
    assert(/variant: DCE 1.1/ =~ UUID4R::uuid_v1(:txt))
    assert(/version: 1/ =~ UUID4R::uuid_v1(:txt))
    assert(/content:/ =~ UUID4R::uuid_v1(:txt))
  end
  
  def test_uuid_v3_str
    a = UUID4R::uuid(3, "ns:URL", "www.sgtpepper.net", :str)
    b = UUID4R::uuid(3, "ns:URL", "www.sgtpepper.net", :str)
    c = UUID4R::uuid_v3("ns:URL", "www.sgtpepper.net", :str)
    d = UUID4R::uuid_v3("ns:URL", "www.sgtpepper.net")
    assert_kind_of(String, a)
    assert(a.length > 0)
    assert_equal(a, b)
    assert_equal(a, c)
    assert_equal(a, d)
  end
  
  def test_uuid_v5_str
    a = UUID4R::uuid(5, "ns:URL", "www.sgtpepper.net", :str)
    b = UUID4R::uuid(5, "ns:URL", "www.sgtpepper.net", :str)
    c = UUID4R::uuid_v5("ns:URL", "www.sgtpepper.net", :str)
    d = UUID4R::uuid_v5("ns:URL", "www.sgtpepper.net")
    assert_kind_of(String, a)
    assert(a.length > 0)
    assert_equal(a, b)
    assert_equal(a, c)
    assert_equal(a, d)
  end

  def test_import
    uuid = UUID4R::import(:str, "266eb9ae-ea6e-11da-8113-0030134d803d")
    assert_kind_of(UUID4R::UUID4RCommon, uuid)
  end
end


class UUID4Rv1Test < Test::Unit::TestCase
  def setup
    @uuid = UUID4R::UUID4Rv1.new
  end

  def test_export_str
    assert_kind_of(String, @uuid.export(:str))
    assert_equal(@uuid.export, @uuid.export(:str))
  end

  def test_compare
    uuid2 = UUID4R::UUID4Rv1.new
    assert(@uuid.compare(uuid2) == -1)
    assert(@uuid.compare(@uuid) ==  0)
    assert(uuid2.compare(@uuid) ==  1)
    assert_equal(-1, @uuid <=> uuid2)
    assert_equal( 0, @uuid <=> @uuid)
    assert_equal( 1, uuid2 <=> @uuid)
  end
end


class UUID4Rv4Test < Test::Unit::TestCase
  def setup
    @uuid = UUID4R::UUID4Rv4.new
  end

  def test_export_str
    assert_kind_of(String, @uuid.export(:str))
    assert_equal(@uuid.export, @uuid.export(:str))
  end
end


class UUID4Rv3Test < Test::Unit::TestCase
  def setup
    @uuid = UUID4R::UUID4Rv3.new("ns:URL", "www.sgtpepper.net")
  end

  def test_export_str
    assert_kind_of(String, @uuid.export(:str))
    assert_equal(@uuid.export, @uuid.export(:str))
  end
end


class UUID4Rv5Test < Test::Unit::TestCase
  def setup
    @uuid = UUID4R::UUID4Rv5.new("ns:URL", "www.sgtpepper.net")
  end

  def test_export_str
    assert_kind_of(String, @uuid.export(:str))
    assert_equal(@uuid.export, @uuid.export(:str))
  end
end

