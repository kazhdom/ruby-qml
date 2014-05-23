require 'spec_helper'

describe QML::Access do
  class Foo
    include QML::Access

    property :text
    signal :some_signal, [:arg]

    def some_method(a, b)
      a + b
    end
  end

  class Bar < Foo
    def some_method(a, b)
      'overridden'
    end

    def bar_method
      'bar'
    end
  end

  let(:engine) { QML::Engine.new }
  let(:component) do
    QML::Component.new engine, data: <<-EOS
      import QtQuick 2.0
      QtObject {
        id: root
        property string text: foo.text + foo.text
        Connections {
          id: connections
          target: foo
          property var args
          onSome_signal: {
            args = parameters
          }
        }
      }
    EOS
  end
  let(:foo) { Foo.new }
  let(:bar) { Bar.new }
  let(:root) { component.create }

  before do
    engine.context[:foo] = foo
    engine.context[:bar] = bar
  end

  context 'in QML' do
    describe '#some_method' do
      it 'returns value' do
        expect(root.qml_eval('foo.some_method(100, 200)')).to eq 300
      end
    end
    describe 'subclass #some_method' do
      it 'returns value' do
        expect(root.qml_eval('bar.some_method(100, 200)')).to eq 'overridden'
      end
    end
    describe 'name property' do
      it 'can get and set value' do
        root.qml_eval('foo.name = "test"')
        expect(root.qml_eval('foo.name')).to eq 'test'
      end
      it 'can be used for property binding' do
        expect(root.qml_eval('root.text')).to eq 'texttext'
      end
    end
    describe 'some_signal signal' do
      it 'can be connected' do
        foo.some_signal.emit('foo')
        expect(root.qml_eval('connections.args')).to eq ['foo']
      end
    end
  end

  context 'when variadic method is defined' do
    it 'raises error' do
      block = proc do
        Class.new do
          include QML::Access
          def variadic_method(*args)
          end
        end
      end
      expect(&block).to raise_error(QML::AccessError)
    end
  end

end